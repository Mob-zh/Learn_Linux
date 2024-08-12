#include <stdio.h>
#include <stdlib.h>

int main(){

	int c;
	while( (c = fgetc(stdin)) != EOF){
		if(fputc(c,stdout) == EOF){
			perror("fputc()");
			exit(1);
		}
	}
	if(ferror(stdin)){
		perror("fgetc()");
		exit(1);
	}
	exit(0);
}











