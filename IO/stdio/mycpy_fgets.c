#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFERSIZE 1024

int main(int argc,char** argv){

	FILE* fs = NULL, *fd = NULL;
	int temp = 0;
	char buf[BUFFERSIZE];
	if(argc < 3){
		fprintf(stderr,"Usage:%s <src_file> <dest_file>\n",argv[0]);
		exit(1);
	}


	fs = fopen(argv[1],"r+");
	if(fs == NULL){
		perror("fopen()");
		exit(1);
	}
	fd = fopen(argv[2],"w");
        if(fd == NULL){
                perror("fopen()");
                exit(1);
        }

	while(fgets(buf,BUFFERSIZE,fs)!=NULL){

		fputs(buf,fd);	
	}

	fclose(fd);
	fclose(fs);
	exit(0);
}


