#ifndef _RELAYER_H_
#define _RELAYER_H_

#define REL_JOBMAX 10000

enum
{	
	STATE_RUNNING = 1,
	STATE_CANCELED,
	STATE_OVER
};

struct rel_stat_st{
	
	int state;
	int fd1;
	int fd2;
	int64_t count12,count21;
};


int rel_addjob(int fd1,int fd2);
/*
	return >= 0			success
		   == -EINVAL	fail 
		   == -ENOSPC	
		   == -ENOMEM
 */



int rel_canceljob(int id);
/*
	return == 0			success
	return == -EINVAL	fail
	return == -EBUSY	fail,task has been cancel
 */



int rel_waitjob(int id,struct rel_stat_st * ptr);
/*
	return == 0			success
	return == -EINVAL	fail
	
*/

//get job state
int rel_statejob(int id,struct rel_stat_st * ptr);
/*
	return == 0			success
	return == -EINVAL	fail
	
*/


#endif

