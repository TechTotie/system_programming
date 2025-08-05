/*
 * @brief : getopt_long usage
 * extern int optind;
 * extern char * optarg;
 * static int next_char;
 * int getopt_long(int argc,
								   const char * argv[],
			 						 const char * short_options,
 * 								 struct option long_options[],
									 int * longindex);
 * This api should be run in a loop to capture each argument to the program.
 * argv[0] is the program name.
 * each time getopt_long is called, it returns the next_char available, if nothing is available, then -1 is returned.
 * if the next option is not in the list, it will return '?'
 * getopt_long also updates the optindex external variable which points to the next available options on each call.
 * short_options is a string containing a list of options in short like '-h' for --help, '-o:' filename for -- output filename.
 * if the option is followed by input, we need to pass ':' along with it.
 * For options with arguments like -o <filename>, the pointer to the <filename> is set in optarg.
 * and optarg is set to NULL for options without argument
 * long_options is options in long form
*/

#include <getopt.h>
#include <stdlib.h>
#include <stdio.h>

extern int optind;

char * program_name;
/* print message */
void print_message(FILE * stream, int exit_code)
{
	fprintf(stream, "Usage %s [inputs...]\n",program_name);
	fprintf(stream, "-h, --help for help options\n");
	fprintf(stream, "-v, --verbose for verbose\n");
	fprintf(stream, "-o, --output filename for output file\n");
	exit(exit_code);
}

int main(int argc, char *argv[])
{
	const char *short_options = "ho:v";
	struct option long_options[] = {
		{"help", 0, NULL, 'h'},
		{"verbose", 0, NULL, 'v'},
		{"output", 1, NULL, 'o'},
		{NULL, 0, NULL, 0}
	};

	int verbose = 0;
	int next_char = -1;
	program_name = argv[0];
	char * output_filename = NULL;

	do {
		next_char = getopt_long(argc, argv, short_options, long_options, NULL);

		switch(next_char) {
			case 'h' : {
					printf("-h optarg: %s optind: %d\n",optarg, optind);
					print_message(stdout, 0);
				} break;
			case 'o' : {
					printf("-o optarg: %s optind: %d\n",optarg, optind);
					output_filename = optarg;
				} break;
			case 'v' : {
					printf("-v\n");
					verbose = 1;
					} break;
			case '?' : {
					printf("?\n");
					print_message(stderr, 0);
				} break;
			case -1 : {
					printf("-1 Done\n");
				} break;
			default : {
					printf("default\n");
					abort();
				} break; 
		}
	} while(next_char != -1);

	/* Verbose - print rest of arguments after options*/
	if(verbose) {
		int i = optind;
		for(; i < argc; i++) {
			printf(" Argument %s\n",argv[i]);
		}
	}
	return 0;
}
