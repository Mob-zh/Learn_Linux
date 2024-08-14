#include <stdio.h>
#include <stdlib.h>
#include <pwd.h>

int main(int argc,char** argv){

	if(argc < 2){
		printf("Usage: %s  <user_id>\n",argv[1]);
	}
	
	struct passwd *pwdline;

	pwdline = getpwuid(atoi(argv[1]));
	puts(pwdline->pw_name);

	exit(0);

}



