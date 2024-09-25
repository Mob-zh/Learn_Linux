#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <syslog.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#define FNAME "/tmp/out"

//solve daemonsize's problem that can't be 

static FILE* fp = NULL;

int daemonize(){
	
	int fd;
	pid_t pid;
	pid = fork();
	if(pid < 0)
	{
		return -1;
	}
	else if(pid > 0){
		exit(0);
	}

	fd = open("/dev/null",O_RDWR);
	if(fd < 0){
		return -1;
	}

	dup2(fd,0);
	dup2(fd,1);
	dup2(fd,2);

	if(fd > 2){
		close(fd);
	}
	setsid();
	//切换工作路径，防止设备忙
	chdir("/");
	//该进程不会产生新的文件,将umask设为0
	//umask(0);
	return 0;
}

static void daemon_exit(int s){
	//parameter s is signal's name
	

	fclose(fp);
	closelog();
	exit(0);
}

int main(){

	
	struct sigaction sa;

	//signal(SIGINT,daemon_exit);
	//signal(SIGQUIT,daemon_exit);
	
	sa.sa_handler = daemon_exit;
	sigemptyset(&sa.sa_mask);
	sigaddset(&sa.sa_mask,SIGQUIT);
	sigaddset(&sa.sa_mask,SIGTERM);
	sigaddset(&sa.sa_mask,SIGINT);
	sa.sa_flags = 0;
	
	sigaction(SIGINT,&sa,NULL);
	sigaction(SIGQUIT,&sa,NULL);
	sigaction(SIGTERM,&sa,NULL);


	openlog("mydaemon",LOG_PID,LOG_DAEMON);
	if(daemonize()<0){
		syslog(LOG_ERR,"daemonize() failed!");
	}else{
		syslog(LOG_INFO,"daemonize() successded!");

	}

	fp = fopen(FNAME,"w");
	if(fp == NULL){
		syslog(LOG_ERR,"fopen():%s",strerror(errno));
		exit(1);
	}



	int i = 0;
	while(1){
		fprintf(fp,"%d\n",i);
		fflush(fp);
		i++;
		syslog(LOG_DEBUG,"%d is printed",i);
		sleep(1);
	}
	fclose(fp);
	closelog();
	exit(0);
}
