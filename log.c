#include "log.h"

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <sys/time.h>

struct timeval begin;
FILE *file;

void start(const char* filename)
{	
	if (gettimeofday(&begin, NULL) != 0)
		printf("Error getting time!\n");

	file = fopen(filename, "a");

	if (file == NULL)
		printf("Error getting file!\n");
}


double getTime(void) // Gets the elapsed time in milliseconds
{
	struct timeval foobar;

	if (gettimeofday(&foobar, NULL) != 0)
		printf("Error getting time!\n");

	return ((double)(foobar.tv_sec - begin.tv_sec)*1000 + (double)(foobar.tv_usec - begin.tv_usec)/1000);
}


int logPrint(const char* output)
{
	int res = fprintf(file, "%.2f - %u - %s\n", getTime(), getpid(), output);	
	fflush(file);	

	if (res < 0)
		printf("Error writing to log file!\n");

	return res;
}
