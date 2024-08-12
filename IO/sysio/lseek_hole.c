#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#define HOLESIZE 4096
#define FNAME "fhole"

char a[] = "abcdefg";
char A[] = "ABCDEFG";

int main(){
	
	int fd = open(FNAME,O_WRONLY|O_CREAT|O_TRUNC,0600);
	if(fd < 0){
		perror("open()");
		exit(1);
	}
	if(write(fd,a,strlen(a)) != strlen(a)){
		perror("write()");
		exit(1);
	}
	if(lseek(fd,HOLESIZE,SEEK_SET) == -1){
		perror("lseek()");
		exit(1);
	}
	if(write(fd,A,strlen(A)) != strlen(A)){
                perror("write()");
                exit(1);
        }
	

	exit(0);
}







