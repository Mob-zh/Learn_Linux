#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define MINSPARESERVER	5
#define MAXSPARESERVER	10
#define MAXCLIENTS			20

#define SIG_NOTIFY		SIGUSR2

enum{
	
	STATE_IDLE = 0;
	STATE_BUSY;

};

struct server_st{

	pid_t pid;
	int state;
	//int reuse;

};

static struct server_st *serverpool;
static int idle_count = 0,busy_count = 0;


int main(){

	struct sigaction sa,osa;
	int sd;
	int val = 1;
	struct sockaddr_in laddr;
	sa.sa_handler = SIG_ISN;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_NOCLDWAIT;
	sigaction(SIGCHLD,&sa,&osa);
	
	sa.sa_handler = usr2_handler;
	sigempty(&sa.sa_mask);
	sa.sa_flags = 0;
	sigaction(SIG_NOTIFY,&sa,&osa);

	serverpool = mmap(NULL,sizeof(struct server_st)*MAXCLIENTS,PROT_READ|PROT_WRITE,MAP_SHARED|MAP_ANONYMOUS,-1,0);
	if(serverpool == MAP_FAILED){
		perror("mmap()");
		exit(1);
	}


	sd = socket(AF_INET,SOCK_STREAM,0);
	if(sd < 0){
		perror("socket()");
		exit(1);
	}

	if(setsocket(sd,SOL_SOCKET,SO_REUSEADDR,&val,sizeof(val)) < 0)
	{
		perror("setsocket()");
		exit(1);
	}

	laddr.sin_family = AF_INET;
	laddr.sin_port = htons(atoi(SRVERPORT));
	inet_pton(AF_INET,"0.0.0.0",&laddr.sin_addr);

	if(bind(sd,(void*)&laddr,sizeof(laddr)) < 0){
		perror("bind()");
		exit(1);
	}

	
	if(listen(sd,100) < 0){
		perror("listen()");
		exit(1);
	}

	for(){

		fork();
		
	}

	while(){

		sigsuspend();
	}

	exit(0);

}

