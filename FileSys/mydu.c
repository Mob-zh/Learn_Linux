#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <glob.h>

#define PATHSIZE 1024

static int path_noloop(const char* path){
	
	char *pos;
	pos = strrchr(path,'/');
	if(pos == NULL){
		fprintf(stderr,"path_noloop:pos not found");
		exit(1);
	}
	if(strcmp(pos+1,".")==0||strcmp(pos+1,"..")==0){
		return 0;
	}
	return 1;

}

static int64_t mydu(const char *path){

	struct stat statres;
	glob_t globres;
	char nextpath[PATHSIZE];
	int sum,i;
	if(lstat(path,&statres)<0){
		perror("lstat()");
		exit(1);
	}
	//如果是目录
	if(!S_ISDIR(statres.st_mode))
		return statres.st_blocks/2;
	
	//如果不是目录
	strncpy(nextpath,path,PATHSIZE);
	strncat(nextpath,"/*",PATHSIZE-1);
	glob(nextpath,0,NULL,&globres);
	strncpy(nextpath,path,PATHSIZE);
        strncat(nextpath,"/.*",PATHSIZE-1);
        glob(nextpath,GLOB_APPEND,NULL,&globres);
	
	sum = 0;
	//递归每一个文件项
	for(i = 0;i < globres.gl_pathc;i++){
		//隐藏文件中有代表自己的.和上一级目录..形成了回路，要单独剔除
		if(path_noloop(globres.gl_pathv[i])){
			sum += mydu(globres.gl_pathv[i]);
		}
	}
	//加上当前目录文件的大小
	sum += statres.st_blocks/2;
	return sum;
}


int main(int argc, char** argv){
	
	if(argc<2){
		fprintf(stderr,"Usage:%s <path>\n",argv[0]);
		exit(1);
	}
	
	printf("%ld\n",mydu(argv[1]));

	

	exit(1);
}






