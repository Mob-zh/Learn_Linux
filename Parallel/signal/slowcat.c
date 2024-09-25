#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <sys/time.h>

#define CPS 10
#define BUFFERSIZE CPS

static volatile int loop = 0;

static void alrm_handler(int s){
	
	//alarm(1);
	loop = 1;
}
//漏桶(setitimer实现)
int main(int argc,char** argv){
	
	
	if(argc < 2){
		fprintf(stderr,"Usage:%s <filename>\n",argv[0]);
		exit(1);
	}
	
	int fds , fdd = 1;
	int ret ,pos ,len;
	char buffer[BUFFERSIZE];
	struct itimerval itv;
	
	do{
		fds = open(argv[1],O_RDONLY);
		if(fds < 0){
			if(errno != EINTR){
			perror("open()");
			exit(1);
			}

		}
	}while(fds < 0);
	
	signal(SIGALRM,alrm_handler);
    //alarm(1);
	itv.it_interval.tv_sec = 1;
	itv.it_interval.tv_usec = 0;
	itv.it_value.tv_sec = 1;
	itv.it_value.tv_usec = 0;

	if(setitimer(ITIMER_REAL,&itv,NULL)<0)
	{
		perror("setitimer()");
		exit(1);
	}

	while(1)
	{

		while(!loop)
			pause();
		loop = 0;
		
		while(( len = read(fds,buffer,BUFFERSIZE))<0)
		{
			if(errno == EINTR)
				continue;
			perror("read()");
			exit(1);
		}
		if(len == 0){
			break;
		}
		
		pos = 0;

		while(len>0){

			ret = write(fdd,buffer+pos,len);
			if(ret<0){
				if(errno == EINTR)
					continue;
				perror("write()");
				exit(1);
			}
			pos += ret;
			len -= ret;
			

		}
	}

	

	close(fds);
	exit(0);
}

