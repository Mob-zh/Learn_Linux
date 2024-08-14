#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#define TIMESIZE 1024

//学习使用getopt（）函数

/*

-y year
-m month
-d day
-H hour
-M minute
-S second

*/


int main(int argc,char **argv)
{
	time_t stamp;
	char timestr[TIMESIZE+1];
	char timeoutput[TIMESIZE+1];
	struct tm *tm;
	int c = 0;
	FILE *fp = stdout;//一定要初始化
	timestr[0]='\0';
	time(&stamp);
	tm = localtime(&stamp);

	while(1){
	
		c = getopt(argc,argv,"-y:mdH:MS");
		if(c<0) 
			break;
	
		switch(c){
			case 1:
				fp = fopen(argv[optind-1],"w");
				if(fp == NULL){
					perror("fopen()");
					fp = stdout;
				}
				
				break;			
			case 'y':
				if(strcmp(optarg,"2") == 0)
				{
					strncat(timestr,"%y ",TIMESIZE);
				}
				else if(strcmp(optarg,"4") == 0)
					strncat(timestr,"%Y ",TIMESIZE);
				else 
					fprintf(stderr,"Invalid argument for y");
				break;
			case 'm':
				strncat(timestr,"%m ",TIMESIZE);
				break;
			case 'd':
				strncat(timestr,"%d ",TIMESIZE);
			       break;
		       	case 'H':
			       if(strcmp(optarg,"12") == 0)
                                        strncat(timestr,"%I(%P) ",TIMESIZE);
                                else if(strcmp(optarg,"24") == 0)
                                        strncat(timestr,"%H ",TIMESIZE);
                                else
                                        fprintf(stderr,"Invalid argument for H");

				break;
		 	case 'M':
				strncat(timestr,"%M ",TIMESIZE);
				break;
			case 'S':
				strncat(timestr,"%S ",TIMESIZE);
				break;		
			defalt:
				break;
		}
	}

	strncat(timestr,"\n",TIMESIZE);
	strftime(timeoutput,TIMESIZE,timestr,tm);
	fputs(timeoutput,fp);
	if(fp != stdout)
		fclose(fp);
	exit(0);

}






