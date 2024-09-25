#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

#define THRNUM 20

#define FNAME "/tmp/out"
#define BUFSIZE 1024

static pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;

//Use semaphore to restrict access to critical regions

static void * thr_add(void *p){
	
	FILE *fp = NULL;
	char linebuf[BUFSIZE];


	fp = fopen(FNAME,"r+");
	if(fp == NULL)
	{
		perror("fopen()");
		exit(1);
	}
	
	pthread_mutex_lock(&mut);

	fgets(linebuf,BUFSIZE,fp);
	
	fseek(fp,0,SEEK_SET);

	fprintf(fp,"%d\n",atoi(linebuf)+1);

	fclose(fp);

	pthread_mutex_unlock(&mut);

	pthread_exit(NULL);

}

int main(){

	pthread_t tid[THRNUM];
	int err = 0,i = 0;
	for(i = 0;i < THRNUM;i++){

		err = pthread_create(tid+i,NULL,thr_add,NULL);
		if(err){
			
			fprintf(stderr,"pthread_create():%s\n",strerror(err));
			exit(1);
		}

	}
	
	for(i = 0;i<THRNUM;i++){
		pthread_join(tid[i],NULL);
	}
	
	pthread_mutex_destroy(&mut);

	exit(0);
}



