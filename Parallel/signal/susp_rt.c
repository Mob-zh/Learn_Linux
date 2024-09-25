#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>


#define MYRTSIG (SIGRTMIN+6)


static void log_c(){

	write(1,"!",1);

}

int main(){

	int i,j;
	sigset_t set,saveset,oset;
	sigemptyset(&set);

	signal(MYRTSIG,log_c);
	sigaddset(&set,MYRTSIG);
	sigprocmask(SIG_UNBLOCK,&set,&saveset);	
	sigprocmask(SIG_BLOCK,&set,&oset);
	for(j = 0;j<1000;j++){

		for(i = 0;i < 5;i++){

			write(1,"*",1);
			sleep(1);
		}
		write(1,"\n",1);
		//UNBLOCK a sigset and pause()
		sigsuspend(&oset);

		//here is how sigsuspend() implement
		/*
		sigset_t tmpset;
		//oset is UNBLOCK,tmpset is BLOCK
		//so,this code block means unblock first then wait a signal and block again
		sigprocmask(SIG_SETMASK,&oset,&tmpset);
		pause()
		sigprocmask(SIG_SETMASK,&tmpset,NULL);
		*/
	}
	//protect system's status
	sigprocmask(SIG_SETMASK,&saveset,NULL);
	exit(0);

}


