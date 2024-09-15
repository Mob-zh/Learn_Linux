#include <stdio.h>


int a = 10;
int * pa = &a;

static void func(void * ptr){
	
	&ptr = pa;

}

int main(){

	int ptr = 0;
	


	func(&ptr);
	printf("pa:%ll ",pa);
	printf("ptr:%ll\n",ptr);

	return 0;
}

