#include <stdio.h>
#include <glob.h>
#include <stdlib.h>

#define PAT "/etc/*"

#if 0
int errfunc_(const char *errpath,int errno){
	puts(errpath);
	fprintf(stderr,"ERROR MSG:%s\n",strerror(errno));
}
#endif

int main(){

	glob_t globres;
	int err,i;
	if((err = glob(PAT,0,NULL,&globres) )!= 0){
		printf("Error code = %d\n",err);
		exit(1);
	}
	
	for(i = 0;i<globres.gl_pathc;i++){
		puts(globres.gl_pathv[i]);
	}
	
	globfree(&globres);

	exit(0);
}














