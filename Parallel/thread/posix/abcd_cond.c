#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#define THRNUM 4

static int num ;
static void* thr_func(void *p);
static pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

int main(){

	int i = 0,err = 0;
	pthread_t tid[THRNUM];

	//create thread
	for(i = 0;i < THRNUM;i++){

		err = pthread_create(tid+i,NULL,thr_func,(void *)i);
		if(err){
			fprintf(stderr,"pthread_create():%s\n",strerror(err));
		}

	}

	alarm(5);

	for(i = 0;i < THRNUM;i++){
		pthread_join(tid[i],NULL);
	}
	
	pthread_mutex_destroy(&mut);
	pthread_cond_destroy(&cond);
	exit(0);

}


static int next(int n){
	if(n+1 == 4){
		n = 0;
	}
	else n++;
	return n;
}

static void* thr_func(void *p){
	
	int n = (int)p;
	int i = 'a'+(int) p;

	while(1){
		
		pthread_mutex_lock(&mut);
		while(num != n){
			//unlock and wait
			pthread_cond_wait(&cond,&mut);
		}

		printf("%c",i);

		num = next(num);
		//signal and unlock
		pthread_cond_broadcast(&cond);
		pthread_mutex_unlock(&mut);
	}

	pthread_exit(NULL);
}

