#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
int main(int argc, char** argv){

	if(argc<3){
		printf("Usage:mysudo <uid> <command> <parameter...>");
		exit(1);
	}
	pid_t pid;
	//用子进程来做exec，父进程来给他收尸
	pid = fork();
	if(pid<0){
		perror("fork");
		exit(1);
	}
	else if(pid == 0){
		setuid(atoi(argv[1]));
		execvp(argv[2],argv+2);
		perror("execvp()");
		exit(1);
	}
	wait(NULL);

	exit(0);
}




