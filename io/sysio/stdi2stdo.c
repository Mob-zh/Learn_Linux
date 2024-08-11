#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

#define BUFSIZE 4096

int main(void){

	int n;
	char buf[BUFSIZE];
	while((n = read(0,buf,BUFSIZE))>0){
		if(write(1,buf,n) < 0)
			perror("write()");
	}
	if(n<0)
		perror("read()");


	exit(0);
}


