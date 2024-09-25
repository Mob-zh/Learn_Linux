#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>

#define CPS 10
#define BUFFERSIZE CPS
#define BURST 100

//sig_atomic_t 保证该变量的操作在机器上只用一条指令完成
static volatile sig_atomic_t token = 0;

static void alrm_handler(int s){
	
	alarm(1);
	token++;
	if(token > BURST){
		token = BURST;
	}
}
//令牌桶
int main(int argc,char** argv){
	
	
	if(argc < 2){
		fprintf(stderr,"Usage:%s <filename>\n",argv[0]);
		exit(1);
	}
	
	int fds , fdd = 1;
	int ret ,pos ,len;
	char buffer[BUFFERSIZE];
	
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
        alarm(1);

	while(1)
	{

		while(token <= 0)
			pause();
		token --;
		
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

