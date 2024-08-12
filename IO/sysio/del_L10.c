#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>

int main(int argc,char** argv)
{

	if(argc<2){

		printf("Usage:del_L10 <filename>\n");
		exit(1);
	}

	int fd1,fd2;
	fd1 = open(argv[1],O_RDONLY);
	if(fd1<0){
		perror("open()");
		exit(1);
	}
	fd2 = open(argv[1],O_RDWR,0600);
	if(fd2<0){
		perror("open()");
		exit(1);
	}
	int offset10 = 0,offset11 = 0;
	int count = 0;
	char buf;
	while(1){
		int ret = read(fd1,&buf,1);
		if(ret>0){
			if(buf == '\n'){
				count++;	
				if(count == 9){
                        	        offset10 = lseek(fd1,1,SEEK_CUR)+1;
                                	printf("offset10 = %d\n,value = \n",offset10+1);
	                        }
        	                if(count == 10){ 
                	                offset11 == lseek(fd1,1,SEEK_CUR)+1;
					printf("offset11 = %d\n,value = \n",offset11+1);
                	        }

			}
		}
		else if(ret == 0){
			break;
		}
		else{
			perror("read()");
			exit(1);
		}
	}
}




