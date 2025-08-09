/* Program to make a process zombie */

#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>

int main()
{
	const int max = 3;
	pid_t pid[max];
	for(int i = 0; i < max; i++) {
		pid[i] = fork();
		if(pid[i] > 0) {
			sleep(30);
		}
	}
	printf("Main exiting pid:%d\n",(int)getpid());
	return 0;
}
