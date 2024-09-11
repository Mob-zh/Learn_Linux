#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include "mytbf.h"


#define CPS 10
#define BUFFERSIZE 1024
#define BURST 100

static volatile int token = 0;

//令牌桶
//实现mycat的流空功能
int main(int argc,char** argv){
	
	int fds , fdd = 1;
	int ret ,pos ,len;
	int size = 0;
	char buffer[BUFFERSIZE];
	mytbf_t *tbf = NULL;
	if(argc < 2){
		fprintf(stderr,"Usage:%s <filename>\n",argv[0]);
		exit(1);
	}
	
	tbf = mytbf_init(CPS,BURST);
	if(tbf == NULL){
		fprintf(stderr,"mytbf_init() failed!\n");
	}

	do{
		fds = open(argv[1],O_RDONLY);
		if(fds < 0){
			if(errno != EINTR){
			perror("open()");
			exit(1);
			}

		}
	}while(fds < 0);
	

	while(1)
	{
		size = mytbf_fetchtoken(tbf,BUFFERSIZE);
		
		while(( len = read(fds,buffer,size))<0)
		{
			if(errno == EINTR)
				continue;
			perror("read()");
			exit(1);
		}
		if(len == 0){
			break;
		}
		
		if(size - len > 0){
			mytbf_returntoken(tbf,size-len);
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
	
	mytbf_destroy(tbf);
	close(fds);
	exit(0);
}

