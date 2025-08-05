#include <stdio.h>
#include <stdlib.h>
#include "reciprocal.hpp"

int main(int argc, char *argv[])
{
	int i = atoi(argv[1]);
	printf("reciprocal of %d is %f\n", i, reciprocal(i));
	return 0;
}
