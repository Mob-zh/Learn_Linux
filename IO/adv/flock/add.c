#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

#define PROCNUM 20

#define FNAME "/tmp/out"
#define BUFSIZE 1024


//Use semaphore to restrict access to critical regions

static void func_add(void){
	
	FILE *fp = NULL;
	char linebuf[BUFSIZE];


	fp = fopen(FNAME,"r+");
	if(fp == NULL)
	{
		perror("fopen()");
		exit(1);
	}
	
	int fd = fileno(fp);


	lockf(fd,F_LOCK,0);

	fgets(linebuf,BUFSIZE,fp);
	
	fseek(fp,0,SEEK_SET);
	sleep(1);
	fprintf(fp,"%d\n",atoi(linebuf)+1);
	
	fflush(fp);	
	lockf(fd,F_ULOCK,0);
	fclose(fp);

}

int main(){
	pid_t pid = 0;
	int err = 0,i = 0;
	for(i = 0;i < PROCNUM;i++){
		pid = fork();
		if(pid < 0){
			perror("fork()");
			exit(1);
		}
		else if(pid == 0){
			func_add();
			exit(0);
		}

	}
	
	for(i = 0;i<PROCNUM;i++){
		wait(NULL);
	}
	

	exit(0);
}



