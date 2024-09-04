#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
static volatile int loop = 1;

static void alrm_handler(int s){

	loop = 0;
}

int main(){

	int64_t count;
	signal(SIGALRM,alrm_handler);
	alarm(5);
	
	while(loop){
		count ++;
	}
	printf("%lld\n",count);

	exit(0);
}


