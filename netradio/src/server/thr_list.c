#include <stdio.h>
#include <stdlib.h>
#include "thr_list.h"
#include "server_conf.h"

static pthread_t tid_list;
static int nr_list_ent;
static struct mlib_listentry_st *list_ent;

static void *thr_list(void* p){
	int totalsize;
	struct msg_list_st *entlistp;
	struct msg_listentry_st *entryp;

	totalsize = sizeof(chnid_t);
}

int thr_list_create(struct mlib_listentry_st *,int ){


	while(1){
		ret = sendto(serversd,entlistp,totalsize,0,	,	);
		if(ret < 0){
			syslog(LOG_WARNING,"sendto():%s",strerror(errno));
		}
		else{
			syslog(LOG_DEBUG,"sendto():success");
		}
		sleep(1);
	}

}

int thr_list_destroy(void){

}


