#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <string.h>
#include <pthread.h>
#include <math.h>
#include <errno.h>

#include "mytbf.h"

struct mytbf_st{
	int cps;
	int burst;
	int token;
	int pos;	//job数组中的位置
	pthread_mutex_t mut;
	pthread_cond_t cond;
};
static struct mytbf_st *job[MYTBF_MAX];//总的令牌桶数组
static pthread_mutex_t mut_job = PTHREAD_MUTEX_INITIALIZER;//令牌桶数组的互斥量
static pthread_once_t  init_once = PTHREAD_ONCE_INIT;//初始化模块使用的初始化变量

static pthread_t alrm_tid;//时钟线程tid

static int min(int a,int b){
	if(a < b){
		return a;
	}
	else return b;
}

static void *thr_alrm(void *p){
	int i = 0;
	while(1){
		pthread_mutex_lock(&mut_job);
		for(i = 0;i < MYTBF_MAX;i++){
			if(job[i]!=NULL){
				pthread_mutex_lock(&job[i]->mut);

				job[i]->token += job[i]->cps;
				if(job[i]->token > job[i]->burst){
					job[i]->token = job[i]->burst;
				}
				pthread_cond_broadcast(&job[i]->cond);
				pthread_mutex_unlock(&job[i]->mut);
			}
		}
		pthread_mutex_unlock(&mut_job);
		sleep(1);
	}
	return NULL;
}

//模块卸载,卸载掉所有流控结构
static void module_unload(void){
	
	int i;
	pthread_cancel(alrm_tid);
	pthread_join(alrm_tid,NULL);
	for(i = 0;i < MYTBF_MAX;i++){
		free(job[i]);
	}
	return ;
}

//模块装载，只在第一次令牌桶init时执行
//会开启一个定时加令牌的线程
static void module_load(void){
	int err = 0;	
	err = pthread_create(&alrm_tid,NULL,thr_alrm,NULL);
	if(err){
		fprintf(stderr,"pthread_create():%s\n",strerror(errno));
		exit(0);
	}
	//挂钩子
	atexit(module_unload);
}

static int get_free_pos_unlocked(void){

	int i;
	for(i = 0;i < MYTBF_MAX;i++){
		if(job[i]==NULL){
			return i;
		}
	}
	return -1;
}

mytbf_t *mytbf_init(int cps,int burst){
	
	struct mytbf_st *me;
	
	pthread_once(&init_once,module_load);

	me = malloc(sizeof(*me));
	if(me == NULL)
		return NULL;
	me->cps = cps;
	me->burst = burst;
	me->token = 0;
	pthread_mutex_init(&me->mut,NULL);
	pthread_cond_init(&me->cond,NULL);
	
	int pos;
	pthread_mutex_lock(&mut_job);
	pos = get_free_pos_unlocked();
	if(pos < 0){
		pthread_mutex_unlock(&mut_job);
		free(me);
		return NULL;
	}
	me->pos = pos;
	job[me->pos] = me;
	pthread_mutex_unlock(&mut_job);
	return me;
}

//消耗令牌
int mytbf_fetchtoken(mytbf_t *ptr,int size){

	struct mytbf_st *me = ptr;
	
	pthread_mutex_lock(&me->mut);
	
	while(me->token <= 0){
		pthread_cond_wait(&me->cond,&me->mut);
	}
	int n = min(me->token,size);
	me->token -= n;
	pthread_mutex_unlock(&me->mut);

	return n;
}

//还回令牌
int mytbf_returntoken(mytbf_t *ptr,int size){
	struct mytbf_st *me = ptr;
	
	pthread_mutex_lock(&me->mut);

	me->token += size;
	if(me->token > me->burst){
		me->token = me->burst;
	}
	//收回token后通知其他线程可以使用
	pthread_cond_broadcast(&me->cond);

	pthread_mutex_unlock(&me->mut);

	return size;
}

int mytbf_destroy(mytbf_t *ptr){

	struct mytbf_st *me = ptr;
		
	pthread_mutex_lock(&mut_job);
	job[me->pos] = NULL;
	pthread_mutex_unlock(&mut_job);

	pthread_mutex_destroy(&me->mut);
	pthread_cond_destroy(&me->cond);

	free(ptr);
	return 0;
}

