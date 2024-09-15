#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#define THRNUM 4


static void* thr_func(void *p);
static pthread_mutex_t mut[THRNUM];

int main(){

	int i = 0,err = 0;
	pthread_t tid[THRNUM];

	for(i = 0;i < THRNUM;i++){
		
		pthread_mutex_init(mut+i,NULL);

		err = pthread_create(tid+i,NULL,thr_func,(void *)i);
		if(err){
			fprintf(stderr,"pthread_create():%s\n",strerror(err));
		}

	}
	
	pthread_mutex_unlock(mut+0);

	alarm(5);

	for(i = 0;i < THRNUM;i++){
		pthread_join(tid[i],NULL);
	}
	

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

		pthread_mutex_lock(mut+n);

		printf("%c",i);

		pthread_mutex_unlock(mut+next(n));
	}

	pthread_exit(NULL);
}

