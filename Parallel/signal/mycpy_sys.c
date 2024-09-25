#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>

#define BUFFERSIZE 1024

int main(int argc,char** argv){
	
	
	if(argc < 3){
		fprintf(stderr,"Usage:%s <source_filename> <des_filename>",argv[0]);
		exit(1);
	}
	
	int fds , fdd;
	int ret ,pos,len;
	char* buffer;
	do{
		fds = open(argv[1],O_RDONLY);
		if(fds < 0){
			if(errno != EINTR){
			perror("open()");
			exit(1);
			}

		}
	}
	while(fds < 0);
	
	do{
		fdd = open(argv[2],O_WRONLY|O_CREAT|O_TRUNC,0600);
		if(fdd < 0){
			if(errno != EINTR){
				close(fds);
       		 	        perror("open()");
                		exit(1);
			}
        	}
	}while(fdd < 0);
	while(1)
	{
		int len = read(fds,buffer,BUFFERSIZE);
		if(len < 0){
			if(errno == EINTR)
				continue;
			perror("read()");
			exit(1);
		}
		if(len == 0){
			break;
		}

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
	

	close(fdd);
	close(fds);
	exit(0);
}

