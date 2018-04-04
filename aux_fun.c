#include "aux_fun.h"

#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

const char* strContains(const char* str1, const char* str2, const int options[]) // Searches for str2 in str1
{
	int found;
	
	unsigned int i, j;
	for (i = 0; i < strlen(str1); ++i) // Iterator of str1
	{
		found = 1;

		for (j = 0; j < strlen(str2); ++j) // Iterator of str2
		{
			if (options[0]) // -i, disregard case differences ('A' == 'a').
			{
				if (!cmpi(str1[i+j], str2[j])) // Not a match
				{
					found = 0;
					i += j;
					break;
				}
			}
			else
			{
				if (str1[i+j] != str2[j]) // Not a match
				{
					found = 0;
					i += j;
					break;
				}
			}
		}

		if (found) // Found
		{
			if (options[4]) // -w, matches only words, see "man grep" for more info.
			{
				if ((i == 0 || !isWordCharacter(str1[i-1])) && (i + strlen(str2) == strlen(str1)-2 || !isWordCharacter(str1[i+strlen(str2)]))) // grep definition of word
				{
					return str1+i;
				}
				else // Not a word
				{
					i += j;
				}
			}
			else
			{
				return str1+i;
			}
		}
	}

	return NULL;
}

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
