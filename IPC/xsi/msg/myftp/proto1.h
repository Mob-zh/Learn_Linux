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

typedef struct msg_data_st{
	long mtype;
	char data[DATAMAX];
	int datalen;
}msg_data_t;

typedef struct msg_eot_st{
	long mtype;
}msg_eot_t;

union msg_s2c_un{
	long mtype; //what ever who come , always get mtype
	msg_data_t datamsg;
	msg_eot_t eotmsg;
};

#endif

