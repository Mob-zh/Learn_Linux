#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>


int main(){
	
	pid_t pid = fork();
	if(pid < 0){
		perror("fork");
		exit(0);
	}
	else if(pid == 0){
		execl("/bin/sh","sh","-c","date",NULL);
		perror("execl()");
		exit(1);
	}

	wait(0);
	exit(0);

}


