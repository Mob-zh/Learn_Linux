#include <stdio.h>
#include <stdlib.h>

//使用system函数完成的shell

int main(){
	char cmd[1024];
	while(1){
		printf("[MyShell_system]$ ");
		fgets(cmd,1024,stdin);
		system(cmd);
	}
	exit(0);
}

