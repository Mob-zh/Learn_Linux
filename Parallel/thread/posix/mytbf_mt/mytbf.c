#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <pthread.h>
#include "mytbf.h"

#define MYTBF_MAX 1024
static struct sigaction alrm_sa_save;

struct mytbf_st {
	int token;
	int bps;
	int burst;
	int pos;
	pthread_mutex_t mut_tok;
	pthread_cond_t cond_tok;
};

static struct mytbf_st* job[MYTBF_MAX];
static int inited = 0;
static struct sigaction alarm_sa_save;

pthread_t tid_alrm;
static pthread_mutex_t mut_job = PTHREAD_MUTEX_INITIALIZER;
static pthread_once_t init_once = PTHREAD_ONCE_INIT;

static int get_free_pos_unlocked(void){
	int i = 0;
	for(i = 0;i < MYTBF_MAX;i++){
		
		if(job[i] == NULL)
			return i;
	}
	
	return -1;
}

static void* thr_alrm(void* s){

		int i = 0;

		while(1){
			pthread_mutex_lock(&mut_job);
			for(i = 0;i < MYTBF_MAX;i++){
			
				if(job[i] != NULL){

					pthread_mutex_lock(&job[i]->mut_tok);
					job[i]->token += job[i]->bps;
					if(job[i]->token > job[i]->burst)
						job[i]->token = job[i]->burst;

					pthread_cond_broadcast(&job[i]->cond_tok);
					pthread_mutex_unlock(&job[i]->mut_tok);

				}

			}
			pthread_mutex_unlock(&mut_job);
			sleep(1);
		}

}
static void module_unload(void);
static void module_load(void)
{

	int err = 0;

	err = pthread_create(&tid_alrm,NULL,thr_alrm,NULL);
	if(err){
		fprintf(stderr,"pthread_create():%s\n",strerror(err));
	}
	

	//用钩子函数处理退出
	atexit(module_unload);
}

static void module_unload(void)
{
	int i = 0;

	pthread_cancel(tid_alrm);
	pthread_join(tid_alrm,NULL);
	for(i = 0 ;i < MYTBF_MAX;i++){
		if(job[i] != NULL){
			mytbf_destroy(job[i]);
		}
	}

	pthread_mutex_destroy(&mut_job);

}

mytbf_t *mytbf_init(int bps,int burst)
{
	//只在功能开启时注册一次
	
	/*
	lock()
	if(!inited)
	{
		module_load();
		inited = 1;
	}
	unlock()
	*/
	pthread_once(&init_once,module_load);

	struct mytbf_st *me;
	int pos = -1;

	me = malloc(sizeof(*me));
	if(me == NULL)
		return NULL;
	me->token = 0;
	me->bps = bps;
	me->burst = burst;
	me->pos = pos;
	pthread_mutex_init(&me->mut_tok,NULL);
	pthread_cond_init(&me->cond_tok,NULL);

	//critical section should have as less as code
	pthread_mutex_lock(&mut_job);
	pos = get_free_pos_unlocked();
	if(pos < 0){
	
		pthread_mutex_unlock(&mut_job);
		free(me);
		return NULL;
	}
	job[pos] = me;
	pthread_mutex_unlock(&mut_job);

	return me;
}

int min_unlocked(int a,int b){
	if(a>b)
		return b;
	return a;
}


int mytbf_fetchtoken(mytbf_t * ptr,int size)
{
	struct mytbf_st *me = ptr;
	int ret = 0;
	if(size <= 0){
		return -EINVAL;
	}
	//wait new token

	pthread_mutex_lock(&me->mut_tok);
	while(me->token <= 0){

		pthread_cond_wait(&me->cond_tok,&me->mut_tok);
		/*
		pthread_mutex_unlock(&me->mut_tok);
		sched_yield();
		pthread_mutex_lock(&me->mut_tok);
		*/
	}
	ret = min_unlocked(me->token,size);
	me->token -= ret;
	pthread_mutex_unlock(&me->mut_tok);	

	return ret;
}


int mytbf_returntoken(mytbf_t * ptr,int size)
{
	struct mytbf_st* me = ptr;
	if(size <= 0){
		return -EINVAL;
	}

	pthread_mutex_lock(&me->mut_tok);	
	me->token += size;
	if(me->token > me->burst)
		me->token = me->burst;
	pthread_cond_broadcast(&me->cond_tok);
	pthread_mutex_unlock(&me->mut_tok);

	return size;
}


int mytbf_destroy(mytbf_t *ptr)
{
	struct mytbf_st* me = ptr;

	pthread_mutex_lock(&mut_job);
	job[me->pos] = NULL;
	pthread_mutex_lock(&mut_job);

	pthread_mutex_destroy(&me->mut_tok);
	pthread_cond_destroy(&me->cond_tok);

	free(ptr);
	
	return 0;
}

