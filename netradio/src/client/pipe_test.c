#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <proto.h>
#include <signal.h>
#include <arpa/inet.h>
#include <errno.h>
#include <net/if.h>
#include <sys/wait.h>
#include <getopt.h>


#define DEFAULT_PLAYERCMD "/usr/bin/mpg123 -q -" // -q 表示静默模式，- 表示从 stdin 读取





static ssize_t writen(int fd,const char *buf,size_t len){
		
	int ret = 0;
	int pos = 0;
	while(len > 0){
		ret = write(fd,buf+pos,len);
		if(ret < 0){
			if(errno == EINTR){
				continue;
			}
			return -1;
		}
		len -= ret;
		pos += ret;
	}
	return pos;

}

int main(int argc, char *argv[]){

	int pd[2];
	int len = 0;
	pid_t pid;
	

	if(pipe(pd) < 0){
		perror("pipe()");
		exit(-1);
	}

	pid = fork();
	if(pid < 0)
	{
		perror("fork()");
		exit(-1);
	}
	if(pid == 0){//child
        close(pd[1]);//0:read, 1:write
        dup2(pd[0], 0);//set pd[0] as stdin
        if(pd[0] > 0) //close pd[0]
            close(pd[0]);
        /*use shell to parse DEFAULT_PLAYERCMD, NULL means to end*/
        execl("/bin/sh", "sh", "-c", DEFAULT_PLAYERCMD, NULL);
        perror("execl()");
        exit(1);
	}
	//parent
	//父进程：直接读mp3文件,发送给子进程
    close(pd[0]);

	int fd_mp3;
    char buf[1024];
    fd_mp3 = open("/home/mob/Music/Starman.mp3",O_RDONLY);
    if(fd_mp3 < 0){
        perror("open()");
        exit(-1);
    }

	while(1){
		len = read(fd_mp3,buf,sizeof(buf));
        if (len < 0) {
            perror("read()");
            break;
        }
        if (len == 0) {
            fprintf(stdout, "End of file\n");
            break; // EOF
        }
        fprintf(stdout,"mp3 received\n");
        if(writen(pd[1],buf,len) < 0){
            perror("writen()");
            exit(0);
        }
	}
	close(fd_mp3);
    close(pd[1]);
    wait(NULL);


	exit(0);
}




