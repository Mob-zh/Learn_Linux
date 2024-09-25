#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>

#define MEMSIZE 1024

// complete communication between parent and child process
int main(int argc,char **argv){

	char *ptr;
	pid_t pid;

	ptr = mmap(NULL,MEMSIZE,PROT_READ|PROT_WRITE,MAP_SHARED|MAP_ANONYMOUS,-1,0);
	if(ptr == MAP_FAILED){
		perror("map()");
		exit(1);
	}


	pid = fork();
	if(pid < 0){
		perror("fork()");
		//dont forget you have mmap a mem
		munmap(ptr,MEMSIZE);
		exit(1);
	}
	else if(pid == 0){
		strcpy(ptr,"Hello!");
		munmap(ptr,MEMSIZE);
		exit(0);
	}

	wait(NULL);
	puts(ptr);
	munmap(ptr,MEMSIZE);

	exit(0);
}


