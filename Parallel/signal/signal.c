#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

static void log_c(){

	write(1,"!",1);

}

int main(){

	int i;
	
	signal(SIGINT,SIG_IGN);
	signal(SIGINT,log_c);
	for(i = 0;i < 10;i++){

		write(1,"*",1);
		sleep(1);
	}

	exit(0);

}

