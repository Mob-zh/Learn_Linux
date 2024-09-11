#include <stdio.h>
#include <stdlib.h>


static void f1(char *str){
	printf("f1: %s\n",str);
}


static void f2(char *str){
	printf("f2: %s\n",str);
}

int main(){
	int job1 = 0,job2 = 0, job3 = 0;

	puts("Begin!");
	job1 = at_addjob(2,f1,"aaa");
	if(job1 < 0){
		fprintf(stderr,"at_addjob():%s\n",strerror(-job1));
		exit(1);
	}
	job2 = at_addjob(7,f2,"bbb");
	if(job2 < 0){
		fprintf(stderr,"at_addjob():%s\n",strerror(-job2));
		exit(1);
	}
	job3 = at_addjob(5,f1,"ccc");
	if(job3 < 0){
		fprintf(stderr,"at_addjob():%s\n",strerror(-job3));
		exit(1);
	}
	puts("End!");

	while(1){
		write(1,".",1);
		pause();
	}

	exit(0);
}


