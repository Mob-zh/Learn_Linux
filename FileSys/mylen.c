#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

//输出文件的长度

static int flen(const char *fname){
	
	struct stat statres;
	if(stat(fname,&statres)<0){
		perror("stat()");
		exit(1);
	}
	return statres.st_size;
}

int main(int argc ,char** argv){

	if(argc<2){
		fprintf(stderr,"Usage...\n");
		exit(1);
	}
	
	printf("%d %s\n",flen(argv[1]),argv[1]);

	exit(0);

}

















