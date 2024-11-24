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
#include <getopt.h>
#include "client.h"

/*
	-M --mgroup
	-P --port
	-p --player
	-H --help
*/

struct client_conf_st client_conf = {\
		DEFAULT_RCVPORT,\
		DEFAULT_MGROUP,\
		DEFAULT_PLAYERCMD
};

int sd;
struct msg_channel_st *msg_channel;

void handle_sigint(int sig){
	printf("destroy client\n");
	free(msg_channel);
	close(sd);

	exit(0);
}

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

	int index = 0;
	struct option argarr[5] = {{"mgroup",1,NULL,'P'},{"port",1,NULL,'M'},\
								{"player",1,NULL,'p'},{"help",1,NULL,'H'},\
								{NULL,0,NULL,0}};

	int pd[2];
	struct ip_mreqn mreq;
	struct sockaddr_in laddr;
	int chosenid;
	int len = 0,ret = 0;
	pid_t pid;
	char c;
	//解析命令行
	while(1)
	{

		c = getopt_long(argc,argv,"P:M:p:H",argarr,&index);
		if(c < 0){
			break;
		}
		switch(c)
		{
			case 'P':
				//optarg 全局变量存储了参数的值
				client_conf.rcvport = optarg;
				break;
			case 'M':
				client_conf.mgroup = optarg;
				break;
			case 'p':
				client_conf.player_cmd = optarg;
				break;
			case 'H':
				printf("这是提示");
				exit(0);
				break;
			default:
				abort();
				break;
		}
	}
	
	sigset_t sigset;
	struct sigaction sa;
	sigfillset(&sigset);
	sa.sa_handler = handle_sigint;
	sa.sa_flags = 0;

	if(sigaction(SIGINT,&sa,NULL) < 0){
		perror("sigaction()");
		exit(-1);
	}

	sd = socket(AF_INET,SOCK_DGRAM,0);
	if(sd < 0)
	{
		perror("socket()");
		exit(-1);
	}
	
	inet_pton(AF_INET,client_conf.mgroup,&mreq.imr_multiaddr);
	inet_pton(AF_INET,"0.0.0.0",&mreq.imr_address);
	mreq.imr_ifindex = if_nametoindex("ens33");

	if(setsockopt(sd,IPPROTO_IP,IP_ADD_MEMBERSHIP,&mreq,sizeof(mreq))<0)
	{
		perror("setsockopt()");
		exit(-1);
	}
	int val = 1;
	if(setsockopt(sd,IPPROTO_IP,IP_MULTICAST_LOOP,&val,sizeof(val))<0)
	{
		perror("setsockopt()");
		exit(-1);
	}

	laddr.sin_family = AF_INET;
	laddr.sin_port = htons(atoi(client_conf.rcvport));
	inet_pton(AF_INET,"0.0.0.0",&laddr.sin_addr.s_addr);
	if(bind(sd,(void*)&laddr,sizeof(laddr)) < 0)
	{
		perror("bind()");
		exit(-1);
	}

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
		        close(sd);//socket
        close(pd[1]);//0:read, 1:write
        dup2(pd[0], 0);//set pd[0] as stdin
        if(pd[0] > 0) //close pd[0]
            close(pd[0]);
        /*use shell to parse DEFAULT_PLAYERCMD, NULL means to end*/
        execl("/bin/sh", "sh", "-c", client_conf.player_cmd, NULL);
        perror("execl()");
        exit(1);
	}
	//parent
	//父进程：从网络上收包,发送给子进程

	//收节目单
	struct msg_list_st *msg_list;
	struct sockaddr_in serveraddr;
	socklen_t serveraddr_len;
	msg_list = malloc(MSG_LIST_MAX);
	if(msg_list == NULL){
		perror("malloc()");
		exit(-1);
	}
	
	while(1){
	
		len = recvfrom(sd,msg_list,MSG_LIST_MAX,0,(void*)&serveraddr,&serveraddr_len);
		if(len < sizeof(struct msg_list_st)){
			fprintf(stderr,"message is too small\n");
			continue;
		}
		if(msg_list->chnid != LISTCHNID){
			fprintf(stderr,"chnid is not match\n");
			continue;
		}
		break;
	}
	
	//打印节目单并选择频道
	struct msg_listentry_st *pos;
	//printf("msg_list size:%ld\nlen = %d\n",sizeof(msg_list)+len,len);
	for(pos = msg_list->entry;(char*)pos < ((char*)msg_list + len);pos=(void*)((char*)pos) + ntohs(pos->len))
	{
		//printf("pos = %ld \nmax = %ld\npos->len:%d\nnext pos:%ld\n",(void *)pos,((void*)msg_list + len),ntohs(pos->len),(void*)((char*)pos) + ntohs(pos->len));
		printf("channel %d : %s\n",pos->chnid,pos->desc);
	}

	//free(msg_list);
	
	printf("输入频道");
	while(ret < 1){
		ret = scanf("%d",&chosenid);
		if(ret != 1)
			exit(-1);
	}

	//收频道音频包，发送给子进程

	struct sockaddr_in raddr;
	socklen_t raddr_len;
	msg_channel = malloc(MSG_CHANNEL_MAX);
	if(msg_channel == NULL){
		perror("malloc()");
		exit(-1);
	}
	
	raddr_len = sizeof(raddr);
	while(1){
		len = recvfrom(sd,msg_channel,MSG_CHANNEL_MAX,0,(void*)&raddr,&raddr_len);
		if(raddr.sin_addr.s_addr != serveraddr.sin_addr.s_addr || raddr.sin_port != serveraddr.sin_port){
			fprintf(stderr,"Ignore: raddr not match ");
			continue;
		}
		if(len < sizeof(struct msg_channel_st)){
			fprintf(stderr,"Ignore: message too small ");
			continue;
		}

		if(msg_channel->chnid == chosenid){
			fprintf(stdout,"accepted msg:%d received\n",msg_channel->chnid);
			if(writen(pd[1],(char *)msg_channel->data,len-sizeof(chnid_t)) < 0){
				perror("writen()");
				exit(0);
			}
		}
	}
	
	free(msg_channel);
	close(sd);

	exit(0);
}




