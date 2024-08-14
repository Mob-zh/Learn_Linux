#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define TIMESIZE 100

int main()
{
	time_t stamp;
	char timestr[TIMESIZE];
	struct tm *tm;
	stamp = time(NULL);
	tm = localtime(&stamp);
	strftime(timestr,TIMESIZE,"Now:%Y-%m-%d",tm);
	puts(timestr);

	tm->tm_mday += 100;
	(void)mktime(tm);
	strftime(timestr,TIMESIZE,"After 100 day:%Y-%m-%d",tm);
	puts(timestr);


	exit(0);
}






