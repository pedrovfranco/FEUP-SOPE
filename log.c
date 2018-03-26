#include "log.h"

#include <time.h>
#include <stdio.h>
#include <unistd.h>

clock_t beginning;
FILE *file;

void start(const char* filename)
{
	beginning = clock();
	file = fopen(filename, "a");
}

long double getTime(void)
{
	return (long double)(clock() - beginning) / CLOCKS_PER_SEC;
}

int logPrint(const char* output)
{
	int res = fprintf(file, "%.2Lf - %u - %s\n", getTime()*1000, getpid(), output);

	if (res < 0)
		printf("Error writing to log file!\n");

	return res;
}