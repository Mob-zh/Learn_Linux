#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void){

	pid_t pid;
	printf("[%d]Begin\n",getpid());
	//fork前记得加上缓冲刷新
	fflush(NULL);

	pid = fork();
	if(pid<0){
		perror("fork()");
		exit(1);
	}
	if(pid == 0){
		printf("[%d]CHILD\n",getpid());
	}
	else{
		printf("[%d]PARENT\n",getpid());
	}
	
	printf("[%d]End\n",getpid());
	exit(0);
}








