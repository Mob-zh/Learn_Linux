#ifndef _ANYTIMER_H_
#define _ANYTIMER_H_

#define JOB_MAX	1024

typedef void at_jobfunc_t(void*);

//任务完成后不要自行删除，就像进程需要被收尸

int at_addjob(int sec, at_jobfunc_t *jobp, void *arg);
/*
   return >= 0			成功，返回任务id
		  == -EINVAL    失败，参数非法
		  == -ENOSPC    失败，数组满
		  == -ENOMEM    失败，空间分配失败
*/

int at_canceljob(int sec, at_jobfunc_t *jobp, void *arg);
/*
   return == 0			成功，制定任务成功取消
		  == -EINVAL    失败，参数非法
		  == -EBUSY     失败，制定任务已经完成
		  == -ECANCELED    失败，制定任务重复取消
*/

int at_waitjob(int id);
/*
   return == 0			成功，制定任务成功释放
		  == -EINVAL    失败，参数非法
*/

#endif

