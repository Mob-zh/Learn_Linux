#ifndef MYTBF_H
#define MYTBF_H

#define MYTBF_MAX 1024
typedef struct mytbf_st mytbf_t;

typedef void (*sighandler_t)(int);

mytbf_t *mytbf_init(int ,int );

int mytbf_fetchtoken(mytbf_t * ,int );

int mytbf_returntoken(mytbf_t * ,int );

int mytbf_destroy(mytbf_t *);

#endif


