#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>


int main(){
	
	puts("Begin");

	pid_t pid = fork();
	if(pid < 0){
		perror("fork");
		exit(0);
	}
	else if(pid == 0){
		execl("/bin/sleep","sleep","100",NULL);
		perror("execl()");
	}

	wait(0);
	puts("End");
	exit(0);

}


