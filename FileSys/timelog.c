#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define FNAME "/tmp/out"
#define BUFSIZE 1024
int main(){

	time_t stamp;
	struct tm *tm;
	char *buf;
	FILE *fp;
	int count = 0;
	fp = fopen(FNAME,"a+");
	if(fp == NULL){
		perror("fopen()");
		exit(1);
	}

	while(fgets(buf,BUFSIZE,fp) != NULL){
		count++;
	}

	while(1){
		
		time(&stamp);
		tm = localtime(&stamp);
		fprintf(fp,"%-4d%d-%d-%d %d:%d:%d\n",++count,
				tm->tm_year+1900,tm->tm_mon+1,tm->tm_mday,
				tm->tm_hour,tm->tm_min,tm->tm_sec);
		fflush(fp);
		sleep(1);
	}


	fclose(fp);
	exit(0);
}









