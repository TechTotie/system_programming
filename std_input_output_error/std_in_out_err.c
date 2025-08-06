/*
 * @brief: Standard input, output and error
 * The I/O in unix is done using stdin(fd = 0), stdout(fd = 1) and stderr(fd = 2).
 * The stdin and stdout are used by printf and scanf respectively.
 * The stdin, stdout and stderr can be used with standard unix calls like read, write and so on, with their respective fd's.
 * The output of a program can be either console or can be redirected to a file using '>'.
 * % program > output.txt.
 * Usually the stdout(fd = 1) is only redirected to file and stderr(fd = 2) is printed onto console
 * If you want both stdout and stderr both onto file, the use the redirectedion '>' and '&' merge operator as shown
 * % program > output.txt 2>&1
 * stderr(fd = 2) is redirected '>' and merged '&' to stdout(fd = 1), which is already redirected to a file.
 * % program > output.txt 2>&1 | filter
 * '|' is pipe, and 2>&1 should be before pipe if you want both output from stdout and stderr as input for function following the pipe, here filter for example.
 *
 * The stdout is buffered, meaning, the stdout will not directly write to console as soon as we use printf to print something.
 * Instead the output is buffered and written, when the buffer is full, or when the stdout fd is closed, or when the program ends.
 * We can flush the stdout using fflush(stdout);
 *
 * In contrast, the stderr is direct, not buffered, anything printed to stderr is directly written onto console.
 *
 * The cin, cout and cerr are similar to the stdin, stdout and stderr respectively.
 * The endl in cout or cerr will also flush the buffer, each time it is used. So, if for performance purposes, you dont want to flush then use newline cahracter '\n' instead
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
	int i = 10;
	fprintf(stdout,"Running the program\n");
	fprintf(stderr,"No error occured until now!\n");

	/* stdout should not be written onto console until flushing*/	
	do {
		fprintf(stdout,".");
		sleep(1);
		fprintf(stderr,"-");
	} while(--i);
	
	fprintf(stdout, "\nFlushing stdout\n");
	fflush(stdout);
}

