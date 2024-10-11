#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include "proto.h"



int main(int argc,char** argv){

	int sd;
	struct msg_st *sbufp;
	struct sockaddr_in raddr;

	if(argc < 3){
		printf("Usage:%s <IP> <NAME>\n",argv[0]);
		exit(1);
	}
	
	if(strlen(argv[2]) > NAMEMAX){
		fprintf(stderr,"Name too long\n");
		exit(1);
	}
	
	int size = sizeof(struct msg_st) + strlen(argv[2]);
	sbufp = malloc(size);
	if(sbufp == NULL){
		perror("malloc()");
		exit(1);
	}
	
	sd = socket(AF_INET,SOCK_DGRAM,0);
	if(sd < 0){
		perror("socket()");
		exit(1);
	}
	
	//bind()
	
	strcpy(sbufp->name,argv[2]);
	sbufp->math = htonl(rand()%100);
	sbufp->chinese = htonl(rand()%100);


	raddr.sin_family = AF_INET;
	raddr.sin_port = htons(atoi(RCVPORT));
	inet_pton(AF_INET,argv[1],&raddr.sin_addr);

	if(sendto(sd,sbufp,size,0,(void *)&raddr,sizeof(raddr)) < 0){
		perror("sendto()");
		exit(1);
	}
	puts("OK!\n");
	close(sd);

	exit(0);
}




