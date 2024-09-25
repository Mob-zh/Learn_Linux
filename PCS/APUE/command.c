#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define BUFSIZE 1024

int main(void){

	char buf[BUFSIZE];
	int status;
	pid_t pid;

	printf("BEGIN!\n");

	while(fgets(buf,BUFSIZE,stdin)!=NULL){
		if(buf[strlen(buf)-1] == '\n'){
			buf[strlen(buf)-1] = 0;
		}
		
		if((pid = fork()) < 0){
			perror("fork()");
			exit(1);
		}
		else if(pid == 0){
			execlp(buf,buf,(char *)0);
			perror("execlp()");
			exit(1);

		}
		else{
			if(pid = waitpid(pid,&status,0) < 0)
				perror("waitpid()");
		}
		printf("END!\n");
	}


	exit(0);
}

