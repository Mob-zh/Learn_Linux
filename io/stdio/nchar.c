#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc,char**argv){

	if(argc<2){
		printf("Usage:%s <filename>\n",argv[0]);
		exit(1);
	}

	FILE* fp = NULL;
	int count = 0;
	int ch = 0;

        fp = fopen(argv[1],"r");
	if(fp == NULL){
		perror("fopen()");
		exit(1);
	}

	while(fgetc(fp)!=EOF){
		count++;
	}
	printf("this file has %d char\n",count);
	
	fclose(fp);
	exit(0);
}



