#include <stdio.h>
#include <stdlib.h>

#define BUFERSIZE 4096

int main(){
	
	char buf[BUFERSIZE];
	while(fgets(buf,BUFERSIZE,stdin) != NULL){
		if(fputs(buf,stdout) == EOF)
		{
			perror("fputs()");
		}
	}

	if(ferror(stdin)){
		perror("fgets()");
	}

	exit(1);
}









