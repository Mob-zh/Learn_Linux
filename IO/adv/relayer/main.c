#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include "relayer.h"

#define TTY1 "/dev/tty11"
#define TTY2 "/dev/tty12"
#define TTY3 "/dev/tty9"
#define TTY4 "/dev/tty10"

int main(){

	int fd1,fd2,fd3,fd4;
	int job[2];
	fd1 = open(TTY1,O_RDWR);
	if(fd1 < 0){
		perror("open()");
		exit(1);
	}
	write(fd1,"TTY1\n",5);
	fd2 = open(TTY2,O_RDWR|O_NONBLOCK);
	if(fd2 < 0){
		perror("open()");
		exit(1);
	}
	write(fd2,"TTY2\n",5);
	
	job[0] = rel_addjob(fd1,fd2);
	if(job[0] < 0){
		fprintf(stderr,"rel_addjob():%s\n",strerror(-job[0]));
	}

	fd3 = open(TTY3,O_RDWR);
	if(fd3 < 0){
		perror("open()");
		exit(1);
	}
	write(fd3,"TTY3\n",5);
	fd4 = open(TTY4,O_RDWR|O_NONBLOCK);
	if(fd4 < 0){
		perror("open()");
		exit(1);
	}
	write(fd4,"TTY4\n",5);
	
	job[1] = rel_addjob(fd3,fd4);
	if(job[1] < 0){
		fprintf(stderr,"rel_addjob():%s\n",strerror(-job[1]));
	}
	
	while(1){
		pause();
	}

	close(fd1);
	close(fd2);
	close(fd3);
	close(fd4);

	exit(0);
}



