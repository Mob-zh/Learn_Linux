#include <stdio.h>
#include <stdlib.h>
//未完成
int main(){

	char *input_pass;
	struct spwd *shadowline;
	if(argc<2)
	{
		fprintf(stderr,"Usage:...");
		exit(1);
	}
	
	input_pass = getpass("Password");
	shadowline = getspname(argv[1]);
	
	
	crypt(input_pass);


}



















