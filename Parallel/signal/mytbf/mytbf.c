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
#include "mytbf.h"

#define MYTBF_MAX 1024
static struct sigaction alrm_sa_save;

struct mytbf_st {
	int token;
	int bps;
	int burst;
	int pos;

};

static struct mytbf_st* job[MYTBF_MAX];
static int inited = 0;
static struct sigaction alarm_sa_save;


static int get_free_pos(void){
	int i = 0;
	for(i = 0;i < MYTBF_MAX;i++){
		
		if(job[i] == NULL)
			return i;
	}
	
	return -1;
}

static void alrm_action(int s,siginfo_t *infop,void *unused){

		int i = 1;

		if(infop->si_code != SI_KERNEL){
			return ;
		}

		for(i = 0;i < MYTBF_MAX;i++){
			
			if(job[i] != NULL){
				job[i]->token += job[i]->bps;
				if(job[i]->token > job[i]->burst)
					job[i]->token = job[i]->burst;
			}

		}

}
static void module_unload(void);
static void module_load(void)
{

	
	struct sigaction sa;
	struct itimerval itv;

	sa.sa_sigaction = alrm_action;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_SIGINFO;

	sigaction(SIGALRM,&sa,&alrm_sa_save);

	itv.it_interval.tv_sec = 1;
	itv.it_interval.tv_usec = 0;
    itv.it_value.tv_sec = 1;
	itv.it_value.tv_usec = 0;
	setitimer(ITIMER_REAL,&itv,NULL);

	//用钩子函数处理退出
	atexit(module_unload);
}

static void module_unload(void)
{
	int i = 0;

	struct itimerval itv;
	
	sigaction(SIGALRM,&alrm_sa_save,NULL);

	itv.it_interval.tv_sec = 1;
	itv.it_interval.tv_usec = 0;
    itv.it_value.tv_sec = 1;
	itv.it_value.tv_usec = 0;
	setitimer(ITIMER_REAL,&itv,NULL);


	for(i = 0 ;i < MYTBF_MAX;i++){
		free(job[i]);
	}
}

mytbf_t *mytbf_init(int bps,int burst)
{
	//只在功能开启时注册一次
	if(!inited)
	{
		module_load();
		inited = 1;
	}
	
	struct mytbf_st *me;
	int pos = -1;
	pos = get_free_pos();
	if(pos < 0)
		return NULL;

	me = malloc(sizeof(*me));
	if(me == NULL)
		return NULL;
	
	me->token = 0;
	me->bps = bps;
	me->burst = burst;
	me->pos = pos;
	job[pos] = me;

	return me;
}

int min(int a,int b){
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
	while(me->token <= 0)
		pause();

	ret = min(me->token,size);
	me->token -= ret;
	return ret;
}

int mytbf_returntoken(mytbf_t * ptr,int size)
{
	struct mytbf_st* me = ptr;
	if(size <= 0){
		return -EINVAL;
	}

	me->token += size;
	if(me->token > me->burst)
		me->token = me->burst;
	return size;
}
int mytbf_destroy(mytbf_t *ptr)
{
	struct mytbf_st* me = ptr;
	job[me->pos] = NULL;
	free(ptr);
	
	return 0;
}

