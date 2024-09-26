#ifndef _MYPIPE_H_
#define _MYPIPE_H_


#define PIPESIZE		1024
#define MYPIPE_READ		0x00000001UL
#define MYPIPE_WRITE	0x00000002UL

typedef void mypipe_t;

mypipe_t *mypipe_init(void);

int mypipe_register(mypipe_t *,int opmap);

int mypipe_unregister(mypipe_t *,int opmap);

int mypipe_read(mypipe_t *ptr,void *buf,size_t count);

int mypipe_write(mypipe_t *ptr,const void *buf,size_t count);

int mypipe_destroy(mypipe_t *ptr);


#endif

