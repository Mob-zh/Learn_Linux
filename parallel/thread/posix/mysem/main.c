#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include "mysem.h"



#define LEFT 30000000
#define RIGHT 30000200
#define THRNUM (RIGHT-LEFT+1)
#define N 4
//thread find primer

struct thr_arg_st
{
	int n;
};

mysem_t *sem;

static void *thrprimer(void *p);


int main(){
	
	int i = 0,err = 0;

	pthread_t tid[THRNUM];
	struct thr_arg_st *p = NULL;
	void *ptr = NULL;
	
	sem = mysem_init(N);
	if(sem == NULL){
		fprintf(stderr,"mysem_init() failed\n");
		exit(1);
	}
	


	for(i = LEFT; i <= RIGHT; i++){

//		the best choice is to use struct send the para
		p = malloc(sizeof(*p));
		p->n = i;
		
		mysem_sub(sem,1);

		err = pthread_create(tid+i-LEFT,NULL,thrprimer,p);
		if(err){
			fprintf(stderr,"pthread_create():%s\n",strerror(err));
			exit(1);
		}

	}

	//free the space
	for(i = LEFT;i <= RIGHT; i++){
			pthread_join(tid[i-LEFT],&ptr);
			free(ptr);
		}

	mysem_destroy(sem);

	exit(0);
}

static void *thrprimer(void *p){

	int i = 0,j = 0, mark = 0;


	i = ((struct thr_arg_st *)p) -> n;
//	free(p);
	mark = 1;
	for(j = 2;j < i/2 ;j++){
        	if(i % j == 0){
                mark = 0;
                    break;
            }
	}
    if(mark){
		printf("%d is a primer\n",i);
    }
	
	mysem_add(sem,1);

	pthread_exit(p);
}



