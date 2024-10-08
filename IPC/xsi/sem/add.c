#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <errno.h>

#define PROCNUM 20

#define FNAME "/tmp/out"
#define BUFSIZE 1024

int semid;

//Use semaphore to restrict access to critical regions

static void P(void){
	struct sembuf op;

	op.sem_num = 0;
	op.sem_op = -1;
	op.sem_flg = 0;

	while(semop(semid,&op,1) < 0){
		if(errno != EINTR||errno != EAGAIN){
			perror("semop()");
			exit(1);
		}
	}

}

static void V(void){
	
	struct sembuf op;

	op.sem_num = 0;
	op.sem_op = 1;
	op.sem_flg = 0;

	while(semop(semid,&op,1) < 0){
		if(errno != EINTR||errno != EAGAIN){
			perror("semop()");
			exit(1);
		}
	}
}


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

	P();

	fgets(linebuf,BUFSIZE,fp);
	sleep(1);
	fseek(fp,0,SEEK_SET);
	fprintf(fp,"%d\n",atoi(linebuf)+1);
	
	fflush(fp);	
	
	V();

	fclose(fp);

}

int main(){
	pid_t pid = 0;
	int err = 0,i = 0;
	
	semid = semget(IPC_PRIVATE,1,0600);	
	if(semid < 0){
		perror("semget()");
		exit(1);
	}
	
	if(semctl(semid,0,SETVAL,1) < 0){
		perror("semctl()");
		exit(1);
	}

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
	
	semctl(semid,0,IPC_RMID);


	exit(0);
}



