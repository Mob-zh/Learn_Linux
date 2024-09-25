#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <fcntl.h>

//print how many 'a' in a file 
int main(int argc,char **argv){

	int fd = 0,i = 0,count = 0;
	struct stat statres;
	char *str;

	if(argc < 2){
		printf("Usage:%s <filename>\n",argv[0]);
	}
	
	fd = open(argv[1],O_RDONLY);
	if(fd < 0){
		perror("open()");
		exit(1);
	}
	
	fstat(fd,&statres);

	str = mmap(NULL,statres.st_size,PROT_READ,MAP_SHARED,fd,0);
	if(str == MAP_FAILED){
		perror("mmap()");
		exit(1);
	}
	close(fd);

	for(i = 0;i < statres.st_size;i++){

		if(str[i] == 'a'){
			count++;
		}
	}

	printf("count a = %d\n",count);

	munmap(str,statres.st_size);

	exit(0);
}


