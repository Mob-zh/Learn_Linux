#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#define LEFT 30000000
#define RIGHT 30000200

//多进程计算质数
//加入收尸
int main(){
	
	int i,j,mark;

	for(i = LEFT;i<=RIGHT;i++)
	{
		pid_t pid;
		pid = fork();
		if(pid < 0){
			perror("fork()");
			exit(0);
		}
		else if(pid == 0){
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
			//观察孤儿进程
			//sleep(1000);
			exit(0);
		}
	}
	
	//观察僵尸进程
	//sleep(1000);
	//
	for(i = LEFT;i <= RIGHT;i++){
		wait(NULL);
	}
	exit(0);
}




