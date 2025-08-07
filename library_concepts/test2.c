/* Program has some implmentation that is later used to create library */

#include <stdio.h>
void f()
{
	/* some implementation */
	printf("%s %d %s\n",__FILE__,__LINE__,__func__);
}

void g()
{
	/* some implementation */
	printf("%s %d %s\n",__FILE__,__LINE__,__func__);
}
