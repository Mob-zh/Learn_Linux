#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <syslog.h>
#include <string.h>
#include <error.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <glob.h>
#include "server_conf.h"
#include "mytbf.h"
#include "medialib.h"
#include "proto.h"

#define PATHSIZE	1024
#define LINEBUFSIZE 1024
#define MP3_BITRATE 64*1024
#define DEBUG

struct channel_context_st{
	chnid_t chnid;	//chnnel id
	char *desc;		//chnnel desc
	glob_t mp3glob;	
	int pos;		//播放的mp3的位置
	int fd;			//media fd
	off_t offset;	//当前播放位置偏移量
	mytbf_t *tbf;	//流控结构体
};

static struct channel_context_st chnnel[CHNNR+1];

static struct channel_context_st* path2entry(const char *path){

	struct channel_context_st *me;
	char pathstr[PATHSIZE] = {'\0'};
	char linebuf[LINEBUFSIZE];
	static chnid_t curr_id = MINCHNID;
	me = malloc(sizeof(*me));
	if(me == NULL){
		syslog(LOG_ERR,"malloc() error");
		exit(1);
	}
	FILE * fp;
	strcat(pathstr,path);
	strcat(pathstr,"/desc.txt");
	fp = fopen(pathstr,"r");
	//获取描述文件信息
	if(fp == NULL){
		syslog(LOG_INFO,"%s is not a channel dir(can't find desc.txt)",path);
		return NULL;
	}
	if(fgets(linebuf,LINEBUFSIZE,fp) == NULL){
		syslog(LOG_INFO,"%s is not a channel dir(can't get info from desc.txt)",path);
		fclose(fp);
		return NULL;
	}
	fclose(fp);

	//配置channel内容
	me->tbf = mytbf_init(MP3_BITRATE/8,MP3_BITRATE/8*5);
	if(me->tbf == NULL){
		syslog(LOG_ERR,"mytbf_init():%s",strerror(errno));
		free(me);
		return NULL;
	}
	me->desc = strdup(linebuf);
	strncpy(pathstr,path,PATHSIZE);
	strcat(pathstr,"/*.mp3");
	if(glob(pathstr,0,NULL,&me->mp3glob)!=0){
		//未找到mp3文件
		curr_id++;
		syslog(LOG_ERR,"%s is not a channel dir(can'find mp3)",path);
		free(me);
		return NULL;
	}
	me->pos = 0;
	me->offset = 0;
	me->fd = open(me->mp3glob.gl_pathv[me->pos],O_RDONLY);
	if(me->fd < 0){
		syslog(LOG_WARNING,"%s open failed.",me->mp3glob.gl_pathv[me->pos]);
		free(me);
		return NULL;
	}
	me->chnid = curr_id;
	curr_id++;
	syslog(LOG_DEBUG,"path2entry success");

	return me;
}


int mlib_getchnlist(struct mlib_listentry_st **result,int *resnum){
	
	char path[PATHSIZE];
	glob_t globres;
	int i=0,num=0;
	struct mlib_listentry_st *ptr;
	struct channel_context_st *res;

	//初始化频道表
	for(i = 0;i < MAXCHNID+1;i++){
		chnnel[i].chnid = -1;
	}
	snprintf(path,PATHSIZE,"%s/*",server_conf.media_dir);
	
	#ifdef DEBUG
		printf("current medialib path:%s\n",path);	
	#endif

	if(glob(path,0,NULL,&globres) != 0){
		return -1;
	}

	ptr = malloc(sizeof(struct mlib_listentry_st)*globres.gl_pathc);
	if(ptr == NULL){
		syslog(LOG_ERR,"malloc() error");
		exit(1);
	}
	
	for(i = 0;i < globres.gl_pathc;i++){
		res = path2entry(globres.gl_pathv[i]);
		if(res != NULL){
			syslog(LOG_INFO,"path2entry() return:%d %s",res->chnid,res->desc);
			memcpy(chnnel+res->chnid,res,sizeof(*res));
			ptr[num].chnid = res->chnid;
			ptr[num].desc = strdup(res->desc);
			num++;
		}
	}
	*result = realloc(ptr,sizeof(struct mlib_listentry_st)*num);
	if(*result == NULL){
		syslog(LOG_ERR,"realloc() failed");
	}
	*resnum = num;

	return 0;
}

int mlib_freechnlist(struct mlib_listentry_st *result){

	free(result);
	return 0;
}

static int open_next(chnid_t chnid){
	for(int i = 0;i < chnnel[chnid].mp3glob.gl_pathc;i++){
		chnnel[chnid].pos++;

		if(chnnel[chnid].pos == chnnel[chnid].mp3glob.gl_pathc){
			chnnel[chnid].pos = 0;
			break;
		}

		close(chnnel[chnid].fd);
		open(chnnel[chnid].mp3glob.gl_pathv[chnnel[chnid].pos],O_RDONLY);
		if(chnnel[chnid].fd < 0){
			syslog(LOG_WARNING,"open():%s",strerror(errno));
			return -1;
		}
		else{	
			chnnel[chnid].offset = 0;
			return 0;
		}
	}
	syslog(LOG_ERR,"channel %d :there is no success open",chnid);
	return 0;
}

ssize_t mlib_readchn(chnid_t chnid,void *buf,size_t size){
	int tbfsize;
	int len;
	tbfsize = mytbf_fetchtoken(chnnel[chnid].tbf,size);
	
	while(1){
		len = pread(chnnel[chnid].fd,buf,tbfsize,chnnel[chnid].offset);
		if(len < 0){
			syslog(LOG_WARNING,"media file %s pread():failed",chnnel[chnid].mp3glob.gl_pathv[chnnel[chnid].pos]);
			open_next(chnid);
			
		}
		else if(len == 0){
			syslog(LOG_DEBUG,"media file %s over",chnnel[chnid].mp3glob.gl_pathv[chnnel[chnid].pos]);
			open_next(chnid);
		}
		else{	//len > 0
			chnnel[chnid].offset+=len;
			break;
		}
	}
	if(tbfsize - len > 0){
		mytbf_returntoken(chnnel[chnid].tbf,tbfsize-len);
	}
	
	return len;
}

