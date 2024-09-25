#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>


static void *func(void *p){

	puts("thread run");
	pthread_exit(NULL);
}

int main(){

	pthread_t tid;
	int err = 0;
	puts("Begin!");

	err = pthread_create(&tid,NULL,func,NULL);
	if(err){

		fprintf(stderr,"pthread_create():%s\n",strerror(err));
		exit(1);
	}
	
	pthread_join(tid,NULL);
	puts("End!");

	exit(0);

}




