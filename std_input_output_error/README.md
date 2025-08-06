# Standard Input Output

Any program or utility is of importance if it is able to take inputs and produce inputs based on the input.  
Unix provides standard input and output facilities which are present in `<stdio.h>` header.

The input and output methods and devices may vary based on the requirement.
So, in order for the user program to interact with any kind of input and output without much of a binding to the input output devices,  
Unix treats all the input and output devices as files. So, it provides a descriptor for each of the input and output devices.
Since they are treated as files, they can also be called as *File Descriptors*.

The Unix system calls are not binded to any particular input and output devices.
Rather most of the Unix system calls works on file descriptors, like open, close, socket, read, write etc.


The standard C library provides a a wrapper over these Unix system calls and it also provides `FILE` streams to work with the input output devices, which are treated as files.  
Example: The C standard library calls like fopen, fread, fwrite, fclose, etc., all work on `FILE *` streams.
So irrespective of the devices you can use the C standard library calls to operate on the input output devices as files.

The output of a particular program can act as input of another program. To make this happen Unix provides `pipes (|)` in order to connect output of a program to input of another program.  
`% program1 | program2`  
The way the above pipe works is that it connects the standard output of program1 to the standard input of program2.
In order to do that, Unix has to connect the `fd` of standard output of program1 to the `fd` of standard input of program2. Meaning any `write` in program1 on the standard output fd should be `read` by program2 on standard input fd. The pipe internally connects the fds of standard output of program1 and standard input of program2.

Unix provides the following fixed File Descriptor numbers for standard input, output and error.
- Standard input - `fd 0`
- Standard output - `fd 1`
- Standard error - `fd2`

If we run a program on bash console,  
`% program`  
the input will be taken from console using scanf() and output will be printed onto console using printf().
If the program has any errors, it will be printed onto console, and it uses another output fd, which is standard error. This is different from standard output.

If we need to redirect output of any program onto file, use  
`% program > output_file`  
this redirects the standard output of the program to the file named `output_file`.
But if there are any errors in the program, it will still be printed onto console.
If we want to redirect the errors to the program we need to use `2>&1` after redirection to file.  
`% program > output_file 2>&1`  
This will tell the unix system that the standard error stream with fd 2 should also be merged with standard output stream with fd 1. The standard output is already redirected to file, so will the standard error be.
If you want to pass on the standard output as well as error as input to the next program then
`% program1 2>&1 | program1`  



Flushing the output onto the screen/standard output device, might not be simple operation and will come with an overhead.
So it is not good that each time the iuser program writes to standard output stream, it is displayed onto screen.
In order to optimize and avoid writing standard output onto screen every time a printf() is used, the *standard output is buffered* and this buffer will be flushed(meaning the contents will be displayed onto screen) whenever 
- the user program *forcefully flushes the buffer* - Using `flush(int fd) i.e., flush(1)`  or `fflush(FILE *stream) i.e., fflush(stdout)` from C Standard Library  
 or
- when the standard output fd that has been opened is closed - close(1) or fclose(stdoout)
 or
- when the program/process/thread using this buffer exits.


But this is not the case with stderr, the stderr has to be flushed each time, it is written as it might be important for the program to show the error when it occurs rather than buffering the error.
So anything written into stderr using `fprintf(stderr, "<string>",...)` will be immediately displayed onto screen.

So if we have the following program
```
#include <unistd.h>

for(int i = 0; i < 5; i++) {
	printf("-");
	fprintf(stderr,"*");
	sleep(1);
}
```
The output will be
```
*****-----
```
Refer program [std_in_out_err.c](std_in_out_err.c)
Note that the * is printed first and then the -. The * is printed directly as it is written into stderr and - is printed later all at once, when the buffer is flushed onthe exit of program.

