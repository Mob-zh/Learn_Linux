#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include <time.h>
#include <pwd.h>
#include <grp.h>

int main(int argc , char* argv[]){

	char			*lspath;
	int				fd;
	struct stat		fstate;
	DIR				*dp;
	struct dirent	*dirp;
	char			fileProperty[11];
	struct passwd	*usr;
	struct group	*grp;
	unsigned long	fileSize;
	struct tm		*ltime;
	
	if(argc == 1){
		lspath = ".";
	}
	else if(argc == 2){
		sprintf(lspath,"%s",argv[1]);
	}
	else {
		fprintf(stderr,"usage:%s <dirpath|filepath>",argv[0]);
	}
	
//	printf("%s\n",lspath);

	if(lstat(lspath,&fstate)<0){
		perror("lstate");
		exit(1);
	}
	if(S_ISDIR(fstate.st_mode)){
		dp = opendir(lspath);
		if(dp == NULL){
			perror("opendir()");
			exit(1);
		}
		while((dirp = readdir(dp)) != NULL){
			
			memset(fileProperty,'-',sizeof(fileProperty));
			fileProperty[10] = '\0';
			
			stat(dirp->d_name,&fstate);

			switch((fstate.st_mode&S_IFMT)){
				
				case S_IFSOCK: fileProperty[0] = 's';break;
				case S_IFREG: fileProperty[0] = '-';break;
				case S_IFLNK: fileProperty[0] = 'l';break;
				case S_IFDIR: fileProperty[0] = 'd';break;
				case S_IFIFO: fileProperty[0] = 'p';break;
				case S_IFBLK: fileProperty[0] = 'b';break;
				case S_IFCHR: fileProperty[0] = 'c';break;
			}
			
			unsigned int off = 256;
			for(int j = 0;j < 10; j++){
				switch(fstate.st_mode & (off>>j)){
					case S_IRUSR: fileProperty[1] = 'r'; break;
					case S_IWUSR: fileProperty[2] = 'w'; break;
					case S_IXUSR: fileProperty[3] = 'x'; break;
					case S_IRGRP: fileProperty[4] = 'r'; break;
					case S_IWGRP: fileProperty[5] = 'w'; break;
					case S_IXGRP: fileProperty[6] = 'x'; break;
					case S_IROTH: fileProperty[7] = 'r'; break;
					case S_IWOTH: fileProperty[8] = 'w'; break;
					case S_IXOTH: fileProperty[9] = 'x'; break;
				}
			}

			usr = getpwuid(fstate.st_uid);
			grp = getgrgid(fstate.st_gid);
			fileSize = fstate.st_size;
			ltime = localtime(&fstate.st_ctime);
			

			printf("%ld %s %s %s %ld %s %d\n",dirp->d_ino,fileProperty,usr->pw_name,grp->gr_name,fileSize,dirp->d_name,fstate.st_mode);


		}
	}



	exit(0);
}


