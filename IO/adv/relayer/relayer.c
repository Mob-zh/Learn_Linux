#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <pthread.h>
#include <string.h>

#include "relayer.h"


#define BUFSIZE 1024
#define REL_MAXJOB 10000 

enum {
	STATE_R = 1,
	STATE_W,
	STATE_Ex,
	STATE_T
};

struct rel_fsm_st {
	int state;
	int sfd;
	int dfd;
	char buf[BUFSIZE];
	int pos;
	int len;
	char *errstr;
	int64_t count;
};

struct rel_job_st{

	int job_state;
	int fd1;
	int fd2;
	struct rel_fsm_st fsm12,fsm21;
	int save_fd1,save_fd2;
};

static struct rel_job_st *rel_job[REL_JOBMAX];
static pthread_mutex_t mut_job = PTHREAD_MUTEX_INITIALIZER;
static pthread_once_t once_init = PTHREAD_ONCE_INIT;

static void rel_fsm_driver(struct rel_fsm_st* fsm){
	
	switch(fsm->state){
		case STATE_R:
			fsm->len = read(fsm->sfd,fsm->buf,BUFSIZE);
			if(fsm->len == 0){
				fsm->state = STATE_T;
			}
			else if(fsm->len < 0){
				if(errno == EAGAIN){
					fsm->state = STATE_R;
				}
				else {
					fsm->state = STATE_Ex;
					fsm->errstr = "read()";
				}
			}
			else {
				fsm->state = STATE_W;
				fsm->pos = 0;
			}
			break;

		case STATE_W:
			int tmp = 0;
			tmp = write(fsm->dfd,fsm->buf+fsm->pos,fsm->len);
			
			if(tmp < 0){
				if(errno == EAGAIN){
					fsm->state = STATE_W;
				}
				else {
					fsm->errstr = "write()";
					fsm->state = STATE_Ex;
				}
			}
			else{
				fsm->pos += tmp;
				fsm->len -= tmp;
				if(fsm->len == 0){
					fsm->state = STATE_R;
				}
				else if(fsm->len > 0){
					fsm->state = STATE_W;
				}
			}
			break;
		case STATE_Ex:
			perror(fsm->errstr);
			fsm->state = STATE_T;
			break;
		case STATE_T:
			//do sth
			break;
		default:
			abort();
			break;
	}

}


static void* rel_driver_mt(void* p){

	int i = 0;
	while(1){
		pthread_mutex_lock(&mut_job);
		for(i = 0;i < REL_MAXJOB;i++){
			if(rel_job[i]){
				if(rel_job[i]->job_state == STATE_RUNNING){
					rel_fsm_driver(&rel_job[i]->fsm12);
					rel_fsm_driver(&rel_job[i]->fsm21);
					if(rel_job[i]->fsm12.state == STATE_T && rel_job[i]->fsm21.state ==	STATE_T)
						rel_job[i]->job_state = STATE_OVER;
				}
			}
		}
		pthread_mutex_unlock(&mut_job);
	}

}

static void rel_load(void){
	pthread_t tid;
	int err;
	err = pthread_create(&tid,NULL,rel_driver_mt,NULL);
	if(err){
		fprintf(stderr,"pthread_create():%s\n",strerror(err));
		exit(1);
	}
}

static int get_free_pos_unblocked(void){
	int i;
	for(i = 0;i < REL_JOBMAX;i++){
		if(rel_job[i]==NULL){
			return i;
		}
	}
	return -1;
}



int rel_addjob(int fd1,int fd2){
	
	struct rel_job_st *me = NULL;
	int pos;
	pthread_once(&once_init,rel_load);
	
	me = malloc(sizeof(*me));
	if(me == NULL){
		perror("malloc()");
		return -ENOMEM;	
	}
	
	
	me->fd1 = fd1;
	me->fd2 = fd2;
	me->job_state = STATE_RUNNING;
	
	me->save_fd1 = fcntl(me->fd1,F_GETFL);
	fcntl(me->fd1,F_SETFL,O_RDWR|O_NONBLOCK);
	me->save_fd2 = fcntl(me->fd2,F_GETFL);
	fcntl(me->fd2,F_SETFL,O_RDWR|O_NONBLOCK);

	me->fsm12.state = STATE_R;
	me->fsm12.sfd = me->fd1;
	me->fsm12.dfd = me->fd2;

	me->fsm21.state = STATE_R;
	me->fsm21.sfd = me->fd2;
	me->fsm21.dfd = me->fd1;

	pthread_mutex_lock(&mut_job);
	pos = get_free_pos_unblocked();
	if(pos < 0){
		//remember unlock and free
		pthread_mutex_unlock(&mut_job);
		fcntl(me->fd1,F_SETFL,me->save_fd1);
		fcntl(me->fd2,F_SETFL,me->save_fd2);
		free(me);
		return -ENOSPC;
	}
	rel_job[pos] = me;
	pthread_mutex_unlock(&mut_job);
	
	return pos;

}

