#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>



#define LEFT 30000000
#define RIGHT 30000200
#define THRNUM 4


//thread poor find primer

struct thr_arg_st
{
	int n;
};

static pthread_mutex_t mut_num = PTHREAD_MUTEX_INITIALIZER;

static void *thrprimer(void *p);

int num = 0;

int main(){
	
	int i = 0,err = 0;

	pthread_t tid[THRNUM];
	struct thr_arg_st *p = NULL;
	void *ptr = NULL;

	for(i = 0; i < THRNUM; i++){

//		the best choice is to use struct send the para
		p = malloc(sizeof(*p));
		p->n = i;

		err = pthread_create(tid+i,NULL,thrprimer,(void*)i);
		if(err){
			fprintf(stderr,"pthread_create():%s\n",strerror(err));
			exit(1);
		}

	}
	
	//this loop is used to assign tasks to other threads
	for(i = LEFT;i <= RIGHT;i++){
		
		pthread_mutex_lock(&mut_num);
		
		while(num!=0){
			pthread_mutex_unlock(&mut_num);
			//this func can cede the cheduler to another thread
			sched_yield();
			pthread_mutex_lock(&mut_num);
		}
		num = i;
		pthread_mutex_unlock(&mut_num);
	}
	
	//this code indicates that all task has been released
	pthread_mutex_lock(&mut_num);
	while(num!=0){
		pthread_mutex_unlock(&mut_num);
		//this func can cede the cheduler to another thread
		sched_yield();
		pthread_mutex_lock(&mut_num);
	}
	num = -1;
	pthread_mutex_unlock(&mut_num);


	//free the space
	for(i = 0;i < THRNUM; i++){
			pthread_join(tid[i],&ptr);
			free(ptr);
		}

	pthread_mutex_destroy(&mut_num);

	exit(0);
}

static void *thrprimer(void *p){

	int i = 0,j = 0, mark = 0;
	while(1){
		
		pthread_mutex_lock(&mut_num);
		while(num == 0){
			pthread_mutex_unlock(&mut_num);
			//this func can cede the cheduler to another thread
			sched_yield();
			pthread_mutex_lock(&mut_num);
		}	
		if(num == -1){
			pthread_mutex_unlock(&mut_num);
			break;
		}
		i = num;
		num = 0;
		pthread_mutex_unlock(&mut_num);

		mark = 1;
		for(j = 2;j < i/2 ;j++){
				if(i % j == 0){
				    mark = 0;
				    break;
				 }
		}

		if(mark){
			printf("[%d] %d is a primer\n",(int)p,i);
		}
	}

	pthread_exit(NULL);
}




