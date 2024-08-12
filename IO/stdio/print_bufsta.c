#include <stdio.h>
#include <stdlib.h>


void pr_stdio(const char * ,FILE *);

int main(){

	FILE *fp;
	fputs("enter any char\n",stdout);
	if(getchar() == EOF)
		perror("getchar()");
	fputs("one line to stderr\n",stderr);

	pr_stdio("stdin",stdin);
	pr_stdio("stdout",stdout);
	pr_stdio("stderr",stderr);

	if((fp = fopen("tmp","r")) == NULL)
	{
		perror("fopen()");
		exit(1);
	}

	pr_stdio("tmp",fp);

	exit(0);
}

void pr_stdio(const char *name,FILE *fp ){

	fprintf(stdout,"stream = %s, ",name);
	
	if(fp->_IO_file_flags & _IO_UNBUFFERED)
		fprintf(stdout,"unbuffered, ");
	else if(fp->_IO_file_flags & _IO_LINE_BUF)
		fprintf(stdout,"line buffered, ");
	else 
		fprintf(stdout,"fully buffered, ");
	fprintf(stdout,"buffersize = %d\n",fp->_IO_buf_end - fp->_IO_buf_base);
}







