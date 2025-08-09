/* Program to print pid and ppid */
#include <stdio.h>
#include <unistd.h>

int main()
{
	printf("pid of this process: %d\n",(int)getpid());
	printf("parent pid of this process : %d\n",(int)getppid());
}
