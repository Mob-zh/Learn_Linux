#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>

static void* func(void *p){

	int i = 0;
	printf("%d\n",&i);
	while(1)
		pause();
	pthread_exit(NULL);
}

int main(){

	int i = 0;
	int err = 0;
	pthread_t tid;
	pthread_attr_t attr;

	pthread_attr_init(&attr);

	pthread_attr_setstacksize(&attr,1024*1024*1024);


	for(i = 0; ;i++){
	
		err = pthread_create(&tid,NULL,func,NULL);
		if(err){
			fprintf(stderr,"pthread_create() failed:%s\n",strerror(err));
			break;

		}

	}
	printf("%d",i);
	
	pthread_attr_destroy(&attr);

	exit(0);
}


