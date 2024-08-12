#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>


int main(){

	FILE *fp;
	fp = fopen("tmp","w");
	if(fp == NULL){
//通过errno数值来查找错误信息		
//		fprintf(stderr,"fopen() failed!  errno = %d\n",errno);
//perror直接关联errno对应的错误信息，并打印出来
//		perror("fopen()");
//strerror，输入errno输出错误信息；
		fprintf(stderr,"fopen(): %s\n",strerror(errno));
		exit(1);
	}
	puts("OK!");
	
	fclose(fp);

	exit(0);

}





