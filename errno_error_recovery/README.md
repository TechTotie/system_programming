# errno and error recovery


The user programs may encounter error's during runtime, due to various reasons,
1. Invalid input
2. Occurence of an undesirable event
3. Unable to handle a particular situation etc, etc.

The programs should be able to handle the errors and recover from them.
Most of the errors that can be forseen can be handled either with exiting the program or without exiting the program.
But it is better to exit/abort/terminate the program in a gracceful way with proper error message than to end up with a system crash, which is hard to debug.

Example:
If you have a normal check for null pointer and proceed, you might be getting undesirable results.
```
if(pointer != NULL)
{
	// do something
}
// do something
```
But when the pointer is null, we do not know how the program behaves.

So, instead of a normal check, use assert wherever required.
```
assert(pointer != NULL);
```
so that the program crashes if pointer is null.

Similarly C Standard library provides abort(), exit() and other api's to gracefully terminate an execcuting program.

It is also important to know what the error is, especially when you are using system calls.

Standard C Library provides a mechnism in order to catch the errors that are caused by system calls.
Most of the system calls return -1 on failure, and 0 on success.
But calls like malloc will return NULL on failure. It is hard to have same logic to check return value for error while using system calls. That is why Standard C Library provides `errno` and api's to use errno, `strerror(errno)` and `perror()`.

### errno
The errno is a macro, but it seems like a global variable.
The value of errno is updated after every system call. Meaning it is overwritten on every system call, indicating what is the error that has occured.
So, it is always better to have a copy of errno before calling the next system call.
errno indicated a number that is defined as macros in `<errno.h>` header file.
All these macros start with `E` like `EACCESS`, `EPERM`, `EINTR` etc.

errno itself is not sufficient or descriptive enough for the developer to know what error has happened.
Instead of referring to the header file of what each errno stands for, C Standard library provides 2 api's that were mentioned earlier 
- `#include <string.h>  
sterror(errno)` -> Converts the errno to a description of what error is.
- ` #include <stdio.h>  
perror("<string>")` -> No need to pass errno, should be called immediately after a system call or before calling next system call. Pass any string, that is descriptive to user.


Refer [errno_use.c](errno_use.c).
We can use errno to capture the errors from system calls and then print them descriptively.

```
#include <errno.h>
#include <stdio.h>
#include <string.h>
...

int main(int argc, char *argv[])
{
	int fd = open("/random_file",O_RDONLY);
	if(fd == -1) {
		/* First take backup of errno */
		int error_no = errno;
		perror("File open failed errormsg: ");
		fprintf(strerr, "File open failed %s\n",strerror(errno));
		abort();
	}
	close(fd);
}
```
abort() aborts the program and generates a core dump.

In the above example, the system call open fails, if there is no file named `random_file` in `/`.

### system call failure
Similarly for other system calls like `read, write, sync, select, sleep` which are **blocking calls**, these calls can be interuppted by a signal while executing. Then these calls will return -1 with errno set to `EINTR`.
It is upto the programmer to re-execute these system calls in the program.

Consider a system call `chown`, which is used to `change the owner` of a file or a directory.
Let us see how can the error caused by such system call be handled.
There may be various reasons that this call might fail.
The below code snippet shows the various reasons and erroo handling in these cases.
```
int ret = chown(path, user_id, -1);
if(ret != 0) {
	/* chown has failed */
	/* save the error code */
	int error_code = errno;
	/* chown has failed and return value has to be -1 */
	asset(ret == -1);

	switch(error_code) {
	case EPERM: /* Permission denied */
	case EACCESS: /* Not accessible */
	case EROFS: /* Read only file system */
	case ENAMETOOLONG: /* Path is too long */
	case ENOENT: /* Path does not exist */
	case ENOTDIR: /* Not a directory */
		/* Dont abort, Print error message */
		fprintf(stderr, "Error changing ownership: %s\n",strerror(errno));
		break;
	case EFAULT: /* Path containing invalid memory address, probably a bug */
		abort();
	case ENOMEM: /* Ran out of memory */
		fprintf(stderr,"%s\n",strerror(errno));
		exit(1);
	default: /* Some other error, probably a bug as unable to handle or missed some condition */
		abort();		
	}
}
```
The chown will take the path and the user_id to which the owner of the file has to be changed, alogn with the group.

### Resource Management during Errors while reading a file

In a program, we usually do some memory allocations, initializations, open some file descriptors, memory handles, etc., and then proceed with using them throughout. What if there is error sceanrio in between. The program has to take care that these resources that were allocated and descriptors that were opened need to be closed else it will cause memory leaking and locking of resources.
So the user application needs to take care of deallocating and freeing up the resources and handles before exiting the program.

**It is always recommended to have a clean up code, before calling anything like assert(), abort().**  

It will be easier in C++, as there are classes with constructors and destructors which will allocate on constructions and deallocate on destruction. But still the problem when using raw memory still persists.
Example:
```
Class A * a = new A();
if(some_other_condition_fails) {
	abort();
}
```
Even if Classes provide constructors and destructors, still whenever we use raw resources like allocating on heap, opening files, sockets, etc., it is responsibility of programmer to deallocate and free up the allocated and used resources before exiting the program.

Refer program [clean_up_read_from_file.c](clean_up_read_from_file.c) to see an example of cleanup before exiting using abort().

**NOTE**
If we use `exit()` system call the Operating system will reclaim all the resources that were allocated and initialized in the process. But it is always better to have cleanup before exiting the program using any system call.
