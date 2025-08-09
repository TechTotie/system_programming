# Processes

Every running program is a process. An application can be a single running process or may contain many cooperating processes.

Every operating system as soon as it launches/starts, it starts various processes for its normal operation, to interact with user, to make devices/IO ready for operation etc.
Linux has one such initial process called `init` process. This is the first process that is launched by Linux.
Each process has a unique identifier. Such id is called **Process Identifier (pid)**.
The pid of init is `1`. All the other processes are launched by this `init` process.
The init process will launch a coupe of other processes and those processes will launch a couple of other processes and so on.
The processes have a parent child relationship. Thus all the processes in a system will form a tree like structure having a parent.
The `init` is the `root` node. followed by other processes as branches.
Example: The unix shell that we work can be launched directly by init or by a kshell from windowing system.
So each process has a parent. The child can access the parent's id, which is called as `ppid`

## ps

Linux provides a powerful command line tool to see the processes and their status.
```
% ps
```

output
```
    PID TTY          TIME CMD
    401 pts/0    00:00:01 bash
 118359 pts/0    00:00:00 ps
```

Note that any command given on shell will launch it as another process. So ps is also a command that is launched as a process by bash. So ps is also listed in output of ps command.

ps has a list of arguments that can be used to know more about running processes.
`ps -e` lists all the running processes.
`ps -e -o pid,ppid,command` will list all the running processes along with their pid, parent pid, and the command that this process is executing.

```
% ps -e -o pid, ppid, command
    PID    PPID COMMAND
      1       0 /sbin/init
      2       1 /init
....
    399       2 /init
    400     399 /init
    401     400 -bash
....
  21989       1 /usr/libexec/packagekitd
  21994       1 /usr/libexec/polkitd --no-debug
 118899     401 ps -e -o pid,ppid,command
```

Note that ps(pid = 118899) is child of bash (pid = 401), which is inturn child of /init (pid = 400) and so on.

We can view the pid and ppid of the process programmatically as follows
[print_pid.c](print_pid.c)
```
#include <unistd.h>

int main()
{
	printf("pid of this process : %d\n",(int)getpid());
	printf("pid of parent process : %d\n", (int)getppid());
}
```
Note that getpid() and getppid() return `pid_t` but they are typecasted to int.

## Process termination/Killing a process

A process can be terminated/killed by using `kill` command with the pid. This command will send `SIGTERM` signal to the process guven in pid. If the signal is not masked, then the process will be terminated.  
`kill` command can also be used to send other signals to processes.

## Creating a process

Linux has a peculiar way of creating a process. A process is created by cloning the existing process and then replacing the clone process by other process. We can create a process by using
- `system` command - creates a shell process and runs the command which is passed as argument in it. It has security issues and avoid using it.
- `fork` and `exec` - most common way of creating a process. It creates clone of calling process and replaces it with other process.

Refer [system_usage.c](system_usage.c)
```
#include <stdlib.h>

int main()
{
	return system("ls -l /");
}
```

We use the `system` linux call to list the contents of `/` root folder.
`system`, syatem call takes a const char * as an argument which it passes as a command to execve("/bin/sh","-c", command); which it executes after fork.
So system command forks and then replaces clone with shell and runs command in it.

Other way is to fork and exec.
- fork basically duplicates the calling process.
- exec can be used to replace the clone process with any program passed in the arguments.

Consider an example spawn function that will fork and then exec and returns the pid
[fork_exec.c](fork_exec.c)
```
#include <sys/types.h>
#include <unistd.h>

int spawn(char *program, char * const arguments)
{
	pid_t pid;

	/* Duplicate the process */
	pid = fork();

	if(pid != 0) {

		/* Parent process */
		return pid;

	} else {

		/* Child process */
		/* replace the cloned process with other program */

		execvp(program, arguments);

		/* Control should not return from exec */
		int error_code = errno;
		fprintf(stderr, "exec has returned the control back %s\n", strerror(error_code));
		abort();
	}
}

int main()
{
	/* The arguments should start with name of program and end with null */
	char * args[] = {
		"ls",
		"-l",
		"/",
		NULL
	};
	spawn("ls", args);
	printf("main ended\n");
}
```

The programs will not execute in order.
The ouput might be such that main might exit earlier than child process.
We can call `wait(int &status)` to wait for any of the child process to exit and then collect the exit status for the child.
** wait is a blocking call **

If the child process has already exited, we can still collect the exit status by using wait() call.
wait() call clears the exit status of a process.


## Process Scheduling

As said above, the processes can execute in any order. It depends on the CPU scheduling algorithm.
We can assign priorities to the processes inorder to differentiate the CPU time given to the processes.
The process having higher priority will get larger CPU time and viceversa.
Linux gives a command to change the priority of a process.
The priority of the process is called as `niceness` value of the process.
The lesser the niceness value, higher the prioirity of the process.

The niceness value of the process can be changed by using `nice` command.
```
% nice -n <number> <process>
``` 
The -ve value indicates higher prioirty process.
The value passed after -n will be added to the previous niceness value.
So, providing a -ve value will increase the priority of the process.

Example: We need to sort a file but need not be a high priority task.
```
% sort input_file.txt > output_file.txt
```
If we need to reduce the priority of this process, we can give a +ve niceness value to it.
```
% nice -n 10 sort input_file.txt > output_file.txt
```

nice command can change the niceness value of a running program too.

If you want to change the niceness value of a program to -ve value, you need to be root user.


## Killing a process

A process can be used by using the kill command.
The kill command will send `signal` to the process like `SIGKILL` or `SIGTERM` or any other that is passed to the kill command.


** Refer Signals and How to Kill process using Signals [here](../signals/README.md)  
Please read this before proceeding further
**


Once the process is killed, the parent process should know that the child has been killed.
THis is needed to perform a cleanup operation about the exit status of a process.
As described earlier, we use the `wait` call to get the exit status.


## Zombie process
What happens if the parent process does not wait for the child to exit, then the child becomes a `zombie process`.
A zombie process is a process whose parent does not wait for it to exit and will have the clean up operation pending in the process list.
It is the responsibility of the parent process to clean up the exit status of the child process.
If the parent process does not wait for the child process to terminate and parent process terminates before the child process, then the child process becomes orphan and Linux will take care to attach this child process to the root node., i.e., the `init` process.
So once the child is orphan, it will be automatically attached to `init` node.
The `init` process will timely wait for any processes to be clean up using the `wait` system call.

Checkout the zombie program [zombie_process.c] (zombie_process.c)

Note the output of `ps -e -o pid, ppid, stat, cmd` after running the zombie_process.c.
Since the parent exists after forking, the children become zombie's.
You can see the output
```
	pid			ppid	stat	cmd
 188514     401 S    ./a.out
 188515  188514 Z    [a.out] <defunct>
 188653  188514 Z    [a.out] <defunct>
 188786  188514 Z    [a.out] <defunct>
```
Note that the parent process has already exited and is cleaned up.
The rest of these process are all child processes, with `Z` as status field of ps command and marked `<defunct>` in command field of ps command.
The process that is marked `<defunt>` are all dead processes that are not cleaned up by their parent processes.
They will be later cleaned up by `init` process.

The status field has the following values
Z - Zombie
S - Sleep interruptible
X - dead
W -paging
D - uninterruptible sleep or IO
R - Running  
and so on.


** 
The `wait` system call is not specific to any child process. It just waits for any child process of the parent to terminate.
So, it is alway better to save the exit status which is acquired by wait, as the wait may be called again due to exit of another child process. **

Note that most of the times, it is favourable not to wait as it is blocking call and parent will not execute until wait returns.
In that case, we can have a periodic check for wait. But this also has a problem, what if the child exits before this periodic check.
So Linux provides a way for processes to communicate the exits status between parent and child.
Every time a child terminates, the parent is sent a `SIGCHLD` to indicate that the child has stopped or terminated executing.
So the parent can have a `sigaction.sa_handler` associated for this `SIGCHLD` and perform `wait` operation in that `signal_handler` to get the exit status of the child.


Refer [sig_child_wait.c](sig_child_wait.c)
```
void signal_handler(int signum)
{
	int status = -1;
	pid_t pid = wait(&status);

	/* Take a copy as wait may be called once more due to exit of any other child process */

	pid_t pid_copy = pid;
	int statuc_copy = status;

	printf("The child process with pid:%d has terminated with status: %d\n", pid_copy, status_copy);
	return;
}

int main()
{
	...
	struct sigaction sa;
	memset(&sa, sizeof(sa), 0);

	sa.sa_handler = &signal_handler;
	sigaction(SIGCHLD, &sa, NULL);
	fork();
	...
	
}
```

So each time a child process terminates, the signal handler will be called and the exit status of the child process is collected in the wait system call.
The wait system call returns the pid of the child process that has terminated.

We can also wait for a particular child process by using `waitpid`. Formats of wait
```
pid_t pid = wait(int * wait_status);
pid_t pid = waitpid(pid_t pid, int * wait_status, int options);
```

We can also get the exit status of a program with macros provided with wait system call.
`WEXITSTATUS(int wait_status)`  
`WIFEXITED(int wait_status)` and
`WTERMSIG(int wait_status)`
can be used to know if the child has exited normally or abnormally by a signal.

```
int main()
{
	char *argv[] = {
		"ls",
		"-l",
		"/",
		NULL
	};
	const char program[] = "ls";

 	pid_t pid = spawn(program, argv);
	
	int child_exit_status;
	wait(&child_exit_status);

	if(WIFEXITED(child_exit_status)) {
		printf("Child exited normally with exit code: %d\n", WEXITSTATUS(child_exit_status));
	} else {
			printf("Child exited abnoramlly signal: %d\n", WTERMSIG(child_exit_status));
	}
}
```

`wait3` and `wait4` are additional system calls that return the resource usage statics of the process.
```
pid_t wait3(int *wait_status, int options, struct rusage * usage);
pid_t wait4(pid_t pid, int *wait_status, int options, struct rusage * usage);
```
`struct rusage` is a structure that has all the information about a process resource usage.

Resource usage include 
- cpu user time used - `struct timeval user_time`
- cpu system time used - `struct timeval cpu_time`
- IPC messages recived - `long msgrcvd`
- IPC messages sent - `long msgsnd`
- page reclaims - `long minflt`
- page faults - `long maxflt` etc. 
