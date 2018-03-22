#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "aux_fun.h"

int isWordCharacter(const char ch)
{
	if ((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z') || (ch >= '0' && ch <= '9') || ch == '_')
		return 1;
	else
		return 0;
}	

int intlength(int input)
{
	if (input == 0)
		return 1;

	int i = 0;
	
	while (input)
	{
		input /= 10;
		i++;
	}

	return i;
}

int cmpi(char ch1, char ch2) // Compares two characters without case sensitivity.
{
	if (ch1 >= 'A' && ch1 <= 'Z')
	{
		ch1 += 'a' - 'A';
	}

	if (ch2 >= 'A' && ch2 <= 'Z')
	{
		ch2 += 'a' - 'A';
	}

	return (ch1 == ch2);
}

int errorMessage()
{
	printf("Usage: simgrep [OPTION] PATTERN [FILE]\n");
	return 1;
}