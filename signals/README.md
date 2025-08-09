# Signals

Signals are special messages that are sent to the processes. They are used for communicating and manipulating processes.
Signals are bassically integers with special meaning. There are many signals and they are all listed in `/usr/include/bits/signum.h`.To use signals we can just include `<signal.h>`.

Signals are asynchronous.
Processes can use either of the `disposition` mechanism on receipt of a signal.
- disposition using `signal handler`
- `default disposition`
- ignoring the signals

Most of the signals have default disposition of exit the program.
Signals like `SIGBUS` (bus error), `SIGFPE` (floating point exception) and `SIGSEGV` (segmentation fault) have the same disposition mechanism to print error message and abort the program.
The abort() function will also send `SIGABRT` to the process.

Whenever the program exits because of a signal, then the exit code of the program will be 
`eixt(128 + sigal_number)`, where signal number is the number for the occured signal.
So, we can assume that whenever a process is signalled with a signal, the process will check
- if the signal is ignored - then signal is ignored OR
- if there is any `user defined disposition` - then that dispoaition is called OR
- calls the `default disposition` - which usually prints error message and then performs `exit(128 + <occured_signal_number>)`

## kill command
The processes can be signalled using the `kill` command. `kill` command can also be used to kill the process.
```
kill -<signal_name_or_number> <pid>
```
Example:
```
kill -9 <pid>
```
which sends the `SIGKILL` signal to the process with pid. This is similar to
```
kill -KILL <pid>
```
The numbers of the signal can be foudn in `man 7 signal`.
The C standard library api for kill is 
```
#include <sys/types.h>
#include <signal.h>

int kill(int signum, pid_t pid);
```


We can also use the `SIGTERM` to terminate the process.

Difference between `SIGKILL` and `SIGTERM` is
- SIGKILL - will bluntly kill the process.
- SIGTERM - will ask the process to terminate by itself

Signals can also be used to send some user signals, not only system signals.
`SIGUSR1`, `SIGUSR2` are user defined signals that can be used by user programs to signal.
`SIGHUP` is also used for the similar reason. It is used to signal, that the system has woken up.

## sigaction

In order to handle the signals and provide our own `signal disposition`, Standard C library provides `sigaction` function
Refer [signal_disposition_example.c](signal_disposition_example.c)
```
#include <signal.h>
#include <unistd.h>
#include <string.h>

sig_atomic_t signal_count = 0;

void signal_handler(int signal)
{
	++signal_count;	
}

int main()
{
	struct sigaction sa;
	memset(&sa, sizeof(sa));

	sa.sa_handler = signal_handler;
	sigaction(SIGUSR1, &sa, NULL);
	sleep(30);
	printf("No of times SIGUSR1 occured : %d\n",signal_count);
	return 0;
}
```
The above program will count the number of times this process recives the SIGUSR1.
But the program can receive the signal only once as the program will already be sleeping after sigaction, and on receiving the signal, it will interrupt the sleep system call, and then it executes the signal_handler. After executing the signal handler, the control returns back to statement after sleep. So it prints `No of times SIGUSR1 occured : 1` and then exists.

The `system call sigaction` will take the `signal number`, and two parameters both of type `struct sigaction *`, one as input for current disposition, other as output to save previous disposition, in case you want to revert the disposition later.

The `sa_handler` member of `sigaction` specifies what action needs to be taken on occurence of the signal.
It can take either of the 3 values,
- SIG_DFL - default disposition for the signal
- SIG_IGN - ignore the signal
- handler - pointer to signal handler function, `(void(*signal_handler)(int signal_number))`

**NOTE**
In the above program when the signal occurs, the program directly jumps to the signal handler function, without even completing the sleep call. The signal SIGUSR1, may occur again while executing the signal handler function and this may lead to not so good state of the program. So, try to keep very minimal functionality in the signal handler. Do not perform any I/O or any blocking call or any job that takes too much time to complete. Also, inorder not to be in a chaotic state if at all signal_handler is interrupted by any other signal, it is always better to perform atomic operations in the signal handler. Like we are using `sig_atomic_t` in the above code. It ensures that the operation is completed in a single cycle. Most of the integer operations or operations with size less than that of integer are atomic, but it is always better to use atomic types instead of bare datatypes.

We can pass the `SIGUSR1` to the above program by `ps -e -o pid,command` to get to know the pid and then use this pid in
```
kill -USR1 <pid of above program>
```
Note that we need to remove SIG from any signal to send it via kill command.
No need to remember the numbers like 9 for SIGKILL, 15 for SIGTERM.
Instead, we can use -KILL, -TERM, -USR1 etc., to send the process a signal.

To know the pid of the above program, we can include these lines before sigaction or sleep call
```
printf("pid of current program: %d\n",(int)getpid());
flush(stdout);
```
instead of using `ps -e -o pid,command` to know the pid of the program.


