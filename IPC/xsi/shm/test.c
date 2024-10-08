#include <stdio.h>


#define aa a()

int a(void){
	return 5;	
}


int main(){
	
	printf("%d",aa);
	

	return 0;
}


