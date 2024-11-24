#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include "proto.h"
#include "thr_list.h"
#include "server_conf.h"

static pthread_t tid_list;
static int nr_list_ent;//节目单中的节目数量
static struct mlib_listentry_st *list_ent;//节目单结构

static void *thr_list(void* p){
	int totalsize;
	struct msg_list_st *entlistp;
	struct msg_listentry_st *entryp;
	int i,ret,size;
	totalsize = sizeof(chnid_t);

	//计算节目单总大小
	for(i = 0; i < nr_list_ent;i++ ){
		totalsize += sizeof(struct msg_listentry_st)+strlen(list_ent[i].desc);
	}
	entlistp = malloc(totalsize);
	//syslog(LOG_INFO,"totalsize:%d",totalsize);
	if(entlistp == NULL){
		syslog(LOG_ERR,"malloc():%s",strerror(errno));
		exit(1);
	}

	entlistp->chnid = LISTCHNID;
	entryp = entlistp->entry;
	//syslog(LOG_DEBUG, "nr_list_ent:%d\n", nr_list_ent);
	for(i = 0;i < nr_list_ent;i++){
		size = sizeof(struct msg_listentry_st)+strlen(list_ent[i].desc);
		entryp->chnid = list_ent[i].chnid;
		entryp->len = htons(size);
		strcpy((char*)entryp->desc,list_ent[i].desc);
		//syslog(LOG_INFO, "[%d]entryp len :%d,chnid = %d,desc = %s\n",i,ntohs(entryp->len),entryp->chnid,entryp->desc);
		entryp = (void*)(((char *)entryp)+size);
	}

	while(1){
		//syslog(LOG_INFO, "thr_list sndaddr :%d\n", sndaddr.sin_addr.s_addr);
		ret = sendto(serversd,entlistp,totalsize,0,(void*)&sndaddr,sizeof(sndaddr));
		if(ret < 0){
			syslog(LOG_WARNING,"sendto(server,sd,entlistp...)%s",strerror(errno));
		}
		else{
			syslog(LOG_DEBUG,"sendto(server,sd,entlistp...) success");
		}
		sleep(1);
	}

}

int thr_list_create(struct mlib_listentry_st * listp,int nr_ent){

	int err;

	list_ent = listp;
	nr_list_ent = nr_ent;
	
	err = pthread_create(&tid_list,NULL,thr_list,NULL);
	if(err){
		syslog(LOG_ERR,"pthread_create():%s.",strerror(errno));
		exit(1);
	}

	return 0;
}

int thr_list_destroy(void){

	pthread_cancel(tid_list);
	pthread_join(tid_list,NULL);
	return 0;
}


