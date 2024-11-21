#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include "anytimer.h"

enum{
	
	STATE_RUNNING = 1,
	STATE_CANCELD,
	STATE_OVER

};


struct at_job_st{
	
	int job_state;
	int sec;
	int time_remain;
	at_jobfunc_t *jobp;
	void *arg;

}


int at_addjob(int sec, at_jobfunc_t *jobp, void *arg){



}

int at_canceljob(int sec, at_jobfunc_t *jobp, void *arg){


}

int at_waitjob(int id){



}



