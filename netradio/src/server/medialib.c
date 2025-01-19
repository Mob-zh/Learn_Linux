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

#define PATHSIZE 1024
#define LINEBUFSIZE 1024
#define MP3_BITRATE 64 * 1024
#define DEBUG 1

//频道信息结构体（播放系统的核心，记录频道内容以及完成流控）
struct channel_context_st
{
	chnid_t chnid; //chnnel id
	char *desc;	   //chnnel desc
	glob_t mp3glob;
	int pos;	  //播放的mp3的位置
	int fd;		  //media fd
	off_t offset; //当前播放位置偏移量
	mytbf_t *tbf; //流控结构体
};

//系统的频道总信息数组
static struct channel_context_st chnnel[CHNNR + 1];

//将路径解析为频道信息结构体
static struct channel_context_st *path2entry(const char *path)
{

	struct channel_context_st *me;
	char pathstr[PATHSIZE] = {'\0'};
	char linebuf[LINEBUFSIZE];
	//静态变量存储当前搜索到的频道
	static chnid_t curr_id = MINCHNID;

	me = malloc(sizeof(*me));
	if (me == NULL)
	{
		syslog(LOG_ERR, "malloc() error");
		exit(1);
	}
	//读取文件描述信息
	FILE *fp;
	strcat(pathstr, path);
	strcat(pathstr, "/desc.txt");
	fp = fopen(pathstr, "r");
	//获取频道描述信息
	if (fp == NULL)
	{
		syslog(LOG_INFO, "%s is not a channel dir(can't find desc.txt)", path);
		return NULL;
	}
	if (fgets(linebuf, LINEBUFSIZE, fp) == NULL)
	{
		syslog(LOG_INFO, "%s is not a channel dir(can't get info from desc.txt)", path);
		fclose(fp);
		return NULL;
	}
	fclose(fp);

	//配置channel内容
	//流控推荐MP3_BITRATE / 8,改为4
	me->tbf = mytbf_init(MP3_BITRATE / 4, MP3_BITRATE / 4 * 5);
	if (me->tbf == NULL)
	{
		syslog(LOG_ERR, "mytbf_init():%s", strerror(errno));
		free(me);
		return NULL;
	}
	me->desc = strdup(linebuf);
	//读mp3内容
	strncpy(pathstr, path, PATHSIZE);
	strcat(pathstr, "/*.mp3");
	if (glob(pathstr, 0, NULL, &me->mp3glob) != 0)
	{
		//未找到mp3文件，
		curr_id++;
		syslog(LOG_ERR, "%s is not a channel dir(can'find mp3)", path);
		free(me);
		return NULL;
	}
	me->pos = 0;
	me->offset = 0;
	me->fd = open(me->mp3glob.gl_pathv[me->pos], O_RDONLY);
	if (me->fd < 0)
	{
		syslog(LOG_WARNING, "%s open failed.", me->mp3glob.gl_pathv[me->pos]);
		free(me);
		return NULL;
	}
	me->chnid = curr_id;
	curr_id++;
	syslog(LOG_DEBUG, "path2entry success");

	return me;
}

//获取节目单
int mlib_getchnlist(struct mlib_listentry_st **result, int *resnum)
{

	char path[PATHSIZE];
	glob_t globres;
	int i = 0, num = 0;
	struct mlib_listentry_st *ptr;
	struct channel_context_st *res;

	//初始化频道表
	for (i = 0; i < MAXCHNID + 1; i++)
	{
		chnnel[i].chnid = -1;
	}

	//获取所有的频道目录
	snprintf(path, PATHSIZE, "%s/*", server_conf.media_dir);
	if (glob(path, 0, NULL, &globres) != 0)
	{
		syslog(LOG_ERR, " glob fail");
		return -1;
	}
	//syslog(LOG_DEBUG, "current medialib path:%s", path);

	ptr = malloc(sizeof(struct mlib_listentry_st) * globres.gl_pathc);
	if (ptr == NULL)
	{
		syslog(LOG_ERR, "malloc() error");
		exit(1);
	}
	//逐个频道分析是否合法
	for (i = 0; i < globres.gl_pathc; i++)
	{
		res = path2entry(globres.gl_pathv[i]);
		if (res != NULL) //合法
		{
			syslog(LOG_INFO, "path2entry() return:%d %s", res->chnid, res->desc);
			//channel是总的频道信息数组
			memcpy(chnnel + res->chnid, res, sizeof(*res)); //存数组操作
			ptr[num].chnid = res->chnid;
			ptr[num].desc = strdup(res->desc);
			num++;
		}
	}
	//减少总共消耗的堆内存，重新按需分配
	*result = realloc(ptr, sizeof(struct mlib_listentry_st) * num);
	if (*result == NULL)
	{
		syslog(LOG_ERR, "realloc() failed");
	}
	*resnum = num;

	return 0;
}

int mlib_freechnlist(struct mlib_listentry_st *result)
{

	free(result);
	return 0;
}

//库内函数，用于打开当前频道的下一个mp3文件
static int open_next(chnid_t chnid)
{
	for (int i = 0; i < chnnel[chnid].mp3glob.gl_pathc; i++)
	{
		chnnel[chnid].pos++;

		//循环播放
		if (chnnel[chnid].pos == chnnel[chnid].mp3glob.gl_pathc)
		{
			chnnel[chnid].pos = 0;
			syslog(LOG_INFO, "%d channel over and repeat", chnid);
		}

		close(chnnel[chnid].fd);
		chnnel[chnid].fd = open(chnnel[chnid].mp3glob.gl_pathv[chnnel[chnid].pos], O_RDONLY);
		if (chnnel[chnid].fd < 0)
		{
			syslog(LOG_WARNING, "open():%s", strerror(errno));
			return -1;
		}
		else
		{
			chnnel[chnid].offset = 0;
			return 0;
		}
	}
	syslog(LOG_ERR, "channel %d :there is no success open", chnid);
	return 0;
}

//读频道
ssize_t mlib_readchn(chnid_t chnid, void *buf, size_t size)
{
	int tbfsize;
	int len;
	tbfsize = mytbf_fetchtoken(chnnel[chnid].tbf, size);

	while (1)
	{
		len = pread(chnnel[chnid].fd, buf, tbfsize, chnnel[chnid].offset);
		if (len < 0)
		{
			syslog(LOG_WARNING, "media file %s pread():failed", chnnel[chnid].mp3glob.gl_pathv[chnnel[chnid].pos]);
			open_next(chnid);
		}
		else if (len == 0)
		{
			syslog(LOG_DEBUG, "media file %s over", chnnel[chnid].mp3glob.gl_pathv[chnnel[chnid].pos]);
			open_next(chnid);
		}
		else
		{ //len > 0
			chnnel[chnid].offset += len;
			break;
		}
	}
	//返回少读的token
	if (tbfsize - len > 0)
	{
		mytbf_returntoken(chnnel[chnid].tbf, tbfsize - len);
	}

	return len;
}
