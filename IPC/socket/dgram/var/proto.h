#ifndef _PROTO_H_
#define _PROTO_H_

#define RCVPORT	"1989"

//512 is UDP command length 
#define NAMEMAX (512-8-8)


//Variable-length array
struct msg_st{

	uint32_t math;
	uint32_t chinese;
	uint8_t name[1];

}__attribute__((packed));



#endif 

