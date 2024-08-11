#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>


int main(){
	int count = 0;
	FILE *fp = NULL;
	while(1){
		fp = fopen("tmp","w");
	        if(fp == NULL){
                	perror("fopen");
        	        break;
        	}	
		count++;
	}
	printf("count = %d\n",count);
	exit(0);

}





