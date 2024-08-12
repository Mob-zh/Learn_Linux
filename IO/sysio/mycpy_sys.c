#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


#define BUFFERSIZE 1024

int main(int argc,char** argv){
	
	
	if(argc < 3){
		fprintf(stderr,"Usage:%s <source_filename> <des_filename>",argv[0]);
		exit(1);
	}
	
	int fds , fdd;
	int ret ,pos,len;
	char* buffer;
	fds = open(argv[1],O_RDONLY);
	if(fds < 0){
		perror("open()");
		exit(1);
	}

	fdd = open(argv[2],O_WRONLY|O_CREAT|O_TRUNC,0600);
	if(fdd < 0){
		close(fds);
                perror("open()");
                exit(1);
        }
	
	while(1)
	{
		int len = read(fds,buffer,BUFFERSIZE);
		if(len<0)
		{
			perror("read()");
			exit(1);
		}
		if(len == 0){
			break;
		}

		while(len>0){

			ret = write(fdd,buffer+pos,len);
			if(ret<0){
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

