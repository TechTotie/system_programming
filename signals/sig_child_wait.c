/* Program to demonstrate sigchld and wait */

#include <sys/types.h>
#include <signal.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

sig_atomic_t child_status;

sig_atomic_t child_pid;
sig_atomic_t signalled = 0;

void signal_handler(int sig_num)
{
	int status;
	pid_t pid;
	pid = wait(&status);
	child_pid = (int)pid;
	child_status = status;
	signalled = 1;
}

int main()
{
	int max = 3;
	pid_t pid;

	/* First perform sigaction*/

	struct sigaction sa;

	memset(&sa, sizeof(sa), 0);

	sa.sa_handler = signal_handler;
	sigaction(SIGCHLD, &sa, NULL);

	for(int i = 0; i < max; i++) {
		pid = fork();
		if(pid > 0) {
			printf("forked child pid: %d\n",(int)pid);
		} else {
			printf("Child here\n");
			sleep(3);
			break;				
		}
	}
	if(pid != 0) {
		while(1) {
			sleep(1);
			if(signalled) {
				printf("child %d exited with %d status\n", child_pid, child_status);
				signalled = 0;
			}
		}
	}
}

