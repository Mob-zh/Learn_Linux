#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>


#define MEMSIZE 1024

// complete communication between parent and child process
int main(int argc,char **argv){


	pid_t pid;
	int shmid;
	char* ptr;

	shmid = shmget(IPC_PRIVATE,MEMSIZE,0600);
	if(shmid < 0){
		perror("shmget()");
		exit(1);
	}

	pid = fork();
	if(pid < 0){
		perror("fork()");
		exit(1);
	}
	else if(pid == 0){
		ptr = shmat(shmid,NULL,0);
		if(ptr == (void *)-1){
			perror("shmat()");
			exit(1);
		}
	
		strcpy(ptr,"Hello!");

		shmdt(ptr);
		exit(0);
	}

	wait(NULL);
	ptr = shmat(shmid,NULL,0);
	if(ptr == (void *)-1){
		perror("shmat()");
		exit(1);
	}

	puts(ptr);
	shmdt(ptr);
	shmctl(shmid,IPC_RMID,NULL);

	exit(0);
}

