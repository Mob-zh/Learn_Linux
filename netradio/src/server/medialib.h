#ifndef _MEDIALIB_H_
#define _MEDIALIB_H_

#include "proto.h"

struct mlib_listentry_st{
	chnid_t chnid;
	char *desc;
};

int mlib_getchnlist(struct mlib_listentry_st **result,int *resnum);

int mlib_freechnlist(struct mlib_listentry_st *result);

ssize_t mlib_readchn(chnid_t chnid,void *buf,size_t size);

#endif

