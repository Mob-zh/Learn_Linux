#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define FNAME "/tmp/out"

int main(){

	int fd;

	fd = open(FNAME,O_WRONLY|O_CREAT|O_TRUNC,0600);
	if(fd < 0){
		perror("open()");
		exit(1);
	}
	
	//这样写是错的,因为不原子，可能产生冲突
	close(1);//关闭后最小的可用fd为1
	dup(fd);//dup时将oldfd拷贝一份到最小的可用fd位置，即1
	close(fd);//关闭当前fd
	
	//dup2是原子的close+dup
	dup2(fd,1);
	if(fd!=1){
		close(fd);
	}

/************************************/

	puts("hello!");
}



