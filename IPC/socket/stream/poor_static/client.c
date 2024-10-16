#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include "proto.h"

#define IPSTRSIZE 40
#define BUFSIZE 1024

static void client_job(int sd){
}

int main(int argc ,char** argv){

	int sd;
	struct sockaddr_in raddr;
	socklen_t raddr_len;	
	char ipstr[IPSTRSIZE];
	long long stamp;

	if(argc < 2){
		printf("Usage %s:<IP_addres>\n",argv[0]);
		exit(1);
	}

	sd = socket(AF_INET,SOCK_STREAM,0/*IPPROTO_TCP*/);
	if(sd < 0){
		perror("socket()");
		exit(1);
	}
	
	raddr.sin_family = AF_INET;
	raddr.sin_port = htons(atoi(SERVERPORT));
	inet_pton(AF_INET,argv[1],&raddr.sin_addr);

	raddr_len = sizeof(raddr);

	if(connect(sd,(void *)&raddr,sizeof(raddr)) < 0){
		perror("connect()");
		exit(1);
	}

	//recv()
	//change sd to fp
	FILE *fp;
	fp = fdopen(sd,"r+");
	if(fp == NULL){
		perror("fdopen()");
		exit(1);
	}
	
	if(fscanf(fp,FMT_STAMP,&stamp) < 1)
		fprintf(stderr,"Bad format\n");
	else 
		fprintf(stdout,"stamp = %lld\n",stamp);

	fclose(fp);


	exit(0);
}
