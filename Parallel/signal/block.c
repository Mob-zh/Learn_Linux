#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

static void log_c(){

	write(1,"!",1);

}

int main(){

	int i,j;
	sigset_t set,saveset,oset;
	sigemptyset(&set);

	signal(SIGINT,SIG_IGN);
	signal(SIGINT,log_c);
	sigaddset(&set,SIGINT);
	sigprocmask(SIG_UNBLOCK,&set,&saveset);	
	for(j = 0;j<1000;j++){

		sigprocmask(SIG_BLOCK,&set,&oset);
		for(i = 0;i < 10;i++){

			write(1,"*",1);
			sleep(1);
		}
		write(1,"\n",1);
		sigprocmask(SIG_SETMASK,&oset,NULL);
	}
	sigprocmask(SIG_SETMASK,&saveset,NULL);
	exit(0);

}


