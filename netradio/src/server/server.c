#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <syslog.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/socket.h>


#include "server.h"
#include "server_conf.h"
#include <proto.h>

/*
	-M	指定多播组
	-P	指定接收端口
	-F	前台运行
	-D	指定媒体库位置
	-I	指定网络设备
	-H	显示帮助
*/

struct server_conf_st server_conf = {
	.rcvport = DEFAULT_RCVPORT,
	.mgroup = DEFAULT_MGROUP,
	.media_dir = DEFAULT_MEDIADIR,
	.runmode = RUN_DAEMON,
	.ifname = DEFAULT_IF
};

int serversd;
struct sockaddr_in sndaddr;//对端地址

static void daemon_exit(int s){
	
	closelog();
	
}

static int daemonize(void){
	
	pid_t pid;
	pid = fork();
	if(pid < 0){
//		perror("fork()");
		syslog(LOG_ERR,"fork():%s",strerror(errno));
		return -1;
	}
	if(pid > 0){
		//直接关闭父进程
		exit(0);
	}
	
	int fd = open("/dev/null",O_RDWR);
	if(fd < 0){
		syslog(LOG_WARNING,"open():%s",strerror(errno));
		return -2;
	}
	//重定向标准流的位置
	dup2(fd,0);
	dup2(fd,1);
	dup2(fd,2);
	if(fd > 2){
		close(fd);
	}

	//若调用该函数的进程不是进程组的leader，创建一个脱离控制终端的进程
	setsid();

	chdir("/");
	umask(0);

	return 0;
}

static int socket_init(void){
	

	serversd = socket(AF_INET,SOCK,DGRAM,0);
	if(serversd < 0){
		syslog(LOG_ERR,"socket():%s",strerror(errno));
		exit(-1);
	}
	
	struct ip_mreqn mreq;
	inet_pton(AF_INET,server_conf.mgroup,&mreq.imr_multiaddr);
	inet_pton(AF_INET,"0.0.0.0",&mreq.imr_address);
	mreq.imr_ifindex = if_nametoindex(server_conf.ifname);
	
	if(setsockopt(serversd,IPPROTO_IP,IP_MULTICAST_IF,&mreq,sizeof(mreq)) < 0)
	{
		syslog(LOG_ERR,"setsocket():%s)",strerror(errno));
		exit(-1);
	}

	//bind()

	sndaddr.sin_family = AF_INET;
	sndaddr.sin_port = htos(atoi(server_conf.rcvport));
	inet_pton(AF_INET,server_conf.mgroup,sndaddr.sin_addr.s_addr);
	return 0;
}

int main(int argc,char *argv[]){


	//定义信号行为
	struct sigaction sa;
	sa.sa_handler = daemon_exit;
	sigemptyset(&sa.sa_mask);
	sigaddset(&sa.sa_mask,SIGINT);
	sigaddset(&sa.sa_mask,SIGQUIT);
	sigaddset(&sa.sa_mask,SIGTERM);

	
	sigaction(SIGTERM,&sa,NULL);
	sigaction(SIGQUIT,&sa,NULL);
	sigaction(SIGINT,&sa,NULL);


	//开启系统日志便于报错
	openlog("netradio",LOG_PID|LOG_PERROR,LOG_DAEMON);

	/*命令行分析*/
	int c;
	while(1){
		c = getopt(argc,argv,"M:P:FD:I:H");
		if(c < 0){
			break;
		}
		switch(c){
			case 'M':
				break;
			case 'P':
				break;
			case 'F':
				break;
			case 'D':
				break;
			case 'I':
				break;
			case 'H':
				exit(0);
				break;		
			default:
				abort();
				break;
		}
	}


	/*实现守护进程*/
	if(server_conf.runmode == RUN_DAEMON){
		if(daemonize() != 0){
			exit(-1);
		}
	}
	else if(server_conf.runmode == RUN_FOREGROUND){
		/*do nothing*/
	}
	else {
		syslog(LOG_ERR,"EINVAL server_conf.runmode");
		exit(-1);
	}

	/*socket 初始化*/
	socket_init();

	/*获取频道信息*/
	struct mlib_listentry_st *list;
	int list_size;
	int err;
	
	err = milb_getchnlist(list,list_size);
	if(err < 0){
		syslog(LOG_ERR,"milb_getchnlist error");
	}

	/*创建节目单线程*/
	thr_list_create(list,list_size);
	//if error
	
	/*创建频道线程*/
	int i;
	for(i = 0; i < list_size; i++){
		thr_channel_create(list+i);
		//if error
	}

	syslog(LOG_DEBUG,"%d channel threads created.",i);
	
	while(1){
		pause();
	}
	closelog();

	exit(0);
}



