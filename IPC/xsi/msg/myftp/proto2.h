#ifndef _PROTO1_H_
#define _PROTO1_H_

#define KEYPATH "/etc/services"
#define KEYPROJ "a"

#define PATHMAX 1024

enum{
	MSG_PATH = 1,
	MSG_DATA,
	MSG_EOT
};

typedef struct msg_path_st{
	long mtype;
	char path[PATHMAX];   /*ASCII with tail 0*/
}msg_path_t;

typedef struct msg_s2c_st{
	long mtype;
	int datalen;
	/*
		datalen > 0		:data
				= 0		:eot
	*/
	char data[DATAMAX];
}msg_data_t;

#endif

