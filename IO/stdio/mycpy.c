#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc,char** argv){

	FILE* fs = NULL, *fd = NULL;
	int temp = 0;
	
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

	while(1){
		temp = fgetc(fs);
		if(temp == EOF) break;

		fputc(temp,fd);
	}

	fclose(fd);
	fclose(fs);
	exit(0);
}


