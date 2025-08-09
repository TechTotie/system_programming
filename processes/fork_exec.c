/* Program to demonstrate fork and exec */
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>

/* We can write a common function to fork child processes and do exec of programs */
int spawn(const char * program, char * const arguments[])
{
	pid_t pid;
	
	/* Duplicate this process - fork clones the calling process and returns pid as 0 for the child process and pid of child process to the parent process */
	pid = fork();

	if(pid != 0) {
		/* pid returned to parent process is pid of choild process which is non zero */
		printf("This is Parent process \n 1. self pid: %d \n 2. child process pid : %d \n",(int)getpid(), pid);
	} else {
		/* pid returned to child process is 0. This is because pid 0 is not used and hence this number can be used by fork while cloning and later provide it another pid after forking */
		printf("This is Child process \n 1. self pid: %d \n 2. pid return value from fork : %d \n 3. parent process pid : %d\n",(int)getpid(), pid, (int)getppid());
		
		/* execvp - p for program, v for list of char* arguments */
		execvp(program, arguments);
		int error_code = errno;
		fprintf(stderr, "control should not return after exec , but still has returned %s\n", strerror(error_code));
		abort();
	}
	return pid;	
}

int main(int argc, char *argv[])
{
	pid_t pid;
	char * arguments[] = {
		"ls", /* argv[0] should be the name of the program */
		"-l",
		"/",
		NULL	/* The argument list should end with NULL */
	};

	spawn("ls", arguments);
	printf("main program ends here\n");
	return 0;
}
