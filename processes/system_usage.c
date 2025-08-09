/* Program to demonstrate system call usage */
#include <stdio.h>
#include <stdlib.h>

int main()
{
	int ret = system("ls -l /");
	return ret;
}
