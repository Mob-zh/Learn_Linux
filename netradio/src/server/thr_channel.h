#ifndef _THR_CHNNEL_H_
#define _THR_CHNNEL_H_

#include "medialib.h"

int thr_channel_create(struct mlib_listentry_st*);

int thr_channel_destroy(struct mlib_listentry_st*);

int thr_channel_destroyall(void);

#endif


