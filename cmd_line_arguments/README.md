# Command Line Arguments

Every program is usable if it works on the inputs provided by the user.
If the program is not able to take inputs, then the program might not be flexible enough to work on other inputs.  
So the programs can read the input in a variety of ways
- From a stored file
- From a predefined memory location
- From Command Line Arguments

In this section, we discuss about the way to process the command line arguments.

A program can function differently based on the options.
Eg: `gcc --help` will display how to use gcc along eith a list of options that can be used with gcc.  
`gcc -c <source_file.c>`, the option `-c` will only compile the source code  
`gcc source_file1.o source_file2.o -o binary_file`, the option `-o <output_file>` will produce the output with the provided filename.  
For the program to take these input options and behave differently, the program needs to process the input given in the command line arguments. It is always better to provide 'help' menu to advise how to use the program.


Every program has a main function, that takes inputs as  
`int main(int argc, char * argv[])`  
argc corresponds to the number of arguments and argv corresponds to the list of arguments.

The command line arguments can be processed each time manually by the program developer by parsing them using string functions.
But in order to make the process easier, C provides standard library functions, `getopt` family of functions.

I have written a normal program that will parse the command line arguments. [cmd_line_options_manual.c](cmd_line_options_manual.c)
But it is very tedious to write this code each time in a program.
So it is better to use the stand library functions for parsing the command line arguments.[getopt_usage.c](getopt_usage.c)

```
#include <getopt.h>
extern int optind;
extern char * optarg;
int getopt_long(int argc,
                const char *argv[],
                const char *short_options,
                struct option long_options[],
                int * longindex);
```
The arguments of main() function are passed directly to getopt_long() function.
Let us take an example, of our program having 3 options,
* help (--help, -h)
* output (--output filename, -o filename)
* verbose (--verbose, -v)

The options preceeded by single hyphen `-` are called short options, and the once preceeded by double hyphen `--` are called long options.
```
Short options for our program can be written as shown
const char short_options[] = "ho:v";

struct option {
	const char * name;
	int has_arguments;
	int * flag;
	int val;
};

Long options for our program can be written as shown
struct option long_options[] = {
	{"help", 0, NULL, 'h'},
	{"output", 1, NULL, 'o'},
	{"verbose", 0, NULL, 'v'},
	{NULL, 0, NULL, 0},
};

The long_options struct needs to be ended with NULL and 0.
```

The getopt_long has to be called recursively, until all the arguments are consumed. Each time getopt_long is called, it returns the `next_char` following the hyphen(either single or double).  If the returned `next_char` is not present in short options, then it returns `'?'`. 

An option which is followed by input argument, for eg:, in our program the `-o` or `--output` is followed by output_filename. Such option is given in short option with option followed by ':', for eg: in our program `o:`'. The argument followed by option if any will be contained in `optarg` argument. Similarly the `has_arguments` in long option has to be passed as 1 for any option that is followed by argument. The has_arguments in long option for any option that is not followed by any argument has to be passed as 0. Also the optarg for will be NULL for any option without any argument.

Once the options are consumed, the getopt_long will return -1.

There might be arguments to the program that are not arguments to any options. Once the processing of all the command line arguments are done and if they are followed by command line arguments that are not related to options.
There is a variable referred by getopt functions.  
`extern int optind`  
this is the variable that holds the position that follows the arguments that are not related to any options.
Each time optind is set to position, that follows the arguments related to options.
Example:  
`% ./getopt_usage.out -o output hello there -v hey -h hello hi`  
will print optind as 3 and then optind as 8, thus resetting optind to the next non-option argument.


Using getopt family of functions is a bit complex, but it is handy compared to writing parsing of all the command line options.









