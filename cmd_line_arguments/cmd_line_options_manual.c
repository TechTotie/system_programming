#include <stdio.h>
#include <string.h>

#define MAX_STRING 24
int main(int argc, char* argv[])
{
	char *posn  = NULL;
	const int no_of_options = 2;
	char avail_options[][MAX_STRING] = {"help", "output"};
	unsigned char input_options[no_of_options], temp_output[no_of_options];
	register int i = 0, k = 0;
	int len = 0;

	/* Check if options are given or not*/
	if(argc < 2) {
		printf("Use the following options to run this utility\n");
		printf("-h, --help\t For help\n");
		printf("-o, --output\t To change output filename\n");
		return 0;
	}
	memset(input_options, 0, no_of_options);
	memset(temp_output, 0, no_of_options);
	/*Identify short hand or long hand*/
	for(i = 1; i < argc; i++) {
		len = strlen(argv[i]);
		if(len == 2) {
			for(k = 0; k < sizeof(avail_options)/sizeof(avail_options[0]); k++) {
				if(argv[i][1] == avail_options[k][0]) {
					++input_options[k];
					temp_output[k] = i;
				}
			}
		} else if(len > 2) {
			for(k = 0; k < sizeof(avail_options)/sizeof(avail_options[0]); k++) {
				if(0 == strcmp(avail_options[k], (const char *)&(argv[i][2]))) {
					++input_options[k];
					temp_output[k] = i;
				}
			}
		} else {
			printf("\n Invalid options given\n");
		}
	}

	for(i = 0; i < sizeof(avail_options)/sizeof(avail_options[0]); i++) {
		if(input_options[i] > 0) {
			printf("Options given [%d] for argv[%d] : %s\n",i, temp_output[i], argv[temp_output[i]]);
		}
	}

}

