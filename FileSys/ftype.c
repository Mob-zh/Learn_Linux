#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
static int ftype(const char *fname);

int main(int argc,char *argv[]){

	if(argc<2)
	{
		fprintf(stderr,"Usage:ftype <filename>");
		exit(1);
	}

	printf("%c\n",ftype(argv[1]));
	
	exit(0);
}


static int ftype(const char *fname){

	struct stat statres;
	if(stat(fname,&statres)<0){
		perror("stat()");
		exit(1);
	}

	if(S_ISREG(statres.st_mode))
		return '-';
	else if(S_ISDIR(statres.st_mode))
		return 'd';
		else if(S_ISSOCK(statres.st_mode))
			return 's';
			else if(S_ISCHR(statres.st_mode))
				return 'c';
				else if(S_ISBLK(statres.st_mode))
					return 'b';
					else if(S_ISLNK(statres.st_mode))
						return 'l';
						else if(S_ISFIFO(statres.st_mode))
							return 'p';
						else
							return '?';


}










