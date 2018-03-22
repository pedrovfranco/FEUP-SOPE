#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "dirent.h"

#include "aux_fun.h"

#define BUFFER_SIZE 1024

int options[6] = {0,0,0,0,0,0}; // i, l, n, c, w, r
int fromstdin = 0; // Flag the determines whether the program is reading from a file or from the standard input


const char* strContains(const char* str1, const char* str2) // Searches for str2 in str1
{
	int found;

	for (int i = 0; i < strlen(str1); ++i)
	{
		found = 1;

		for (int j = 0; j < strlen(str2); ++j)
		{
			if (options[0]) // -i, disregard case differences ('A' == 'a').
			{
				if (!cmpi(str1[i+j], str2[j])) // Not a match
				{
					found = 0;
					// i += j;
					break;
				}
			}
			else
			{
				if (str1[i+j] != str2[j]) // Not a match
				{
					found = 0;
					// i += j;
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
			}
			else
			{
				return str1+i;
			}
		}
	}

	return NULL;
}

int mainFunc(const char* pattern, const char* filename)
{
	FILE *file;

	if (fromstdin)
		file = stdin;
	else
		file = fopen(filename, "r");

	if (file == NULL)
	{
		printf("simgrep: %s: No such file or directory\n", filename);
		return 1;
	}

	char *buffer = NULL;
	size_t bufferSize = BUFFER_SIZE;

	unsigned int iteCounter = 0;
	unsigned int matchCounter = 0;


	while(!feof(file))
	{
		getline(&buffer, &bufferSize, file);

		if (buffer[strlen(buffer)-1] != '\n')
					strcat(buffer, "\n");

		if (strContains(buffer, pattern) != NULL)
		{
			matchCounter++;

			if (options[1]) // -l, prints only the filenames of files with matches to pattern.
			{
				printf("%s\n", filename);

				fclose(file);
				return 0;
			}
			else
			{	
				if (options[2]) // -n, iniciates the print with the line number.
				{	
					printf("%u:", iteCounter+1);
				}
				
				printf("%s", buffer);
			}
		}

		iteCounter++;
	}

	fclose(file);

	if (options[3]) // -c, prints the number of matches.
	{
		printf("%u\n", matchCounter);
	}

	return 0;
}

int main(int argc, char const *argv[])
{

	if (argc < 2)
	{
		return errorMessage();
	}

	for (int i = 1; i < argc - 1; ++i) // Fills options array. number of options = argc - 3
	{
		if (argv[i][0] == '-')
		{
			if (strlen(argv[i]) == 2)
			{
				if (strcmp(argv[i], "-i") == 0)
				{
					options[0] = 1;
				}
				else if (strcmp(argv[i], "-l") == 0)
				{
					options[1] = 1;
				}
				else if (strcmp(argv[i], "-n") == 0)
				{
					options[2] = 1;
				}
				else if (strcmp(argv[i], "-c") == 0)
				{
					options[3] = 1;
				}
				else if (strcmp(argv[i], "-w") == 0)
				{
					options[4] = 1;
				}
				else if (strcmp(argv[i], "-r") == 0)
				{
					options[5] = 1;
				}
				else // Not a valid option
				{
					return errorMessage();
				}
			}
			else // Not a valid option
			{
				return errorMessage();
			}
		}
		
	}	

	if (options[5]) // -r, grabs files recursively
	{
		DIR* currdr = opendir(".");
		struct dirent curr;

		if (currdr == NULL)
		{
			printf("opendir error!\n");
			return 1;
		}

		while ((curr = readdir(currdr)) != NULL)
		{
			
		}

	}
	else
	{
		if (argc == 2 || argv[argc-2][0] == '-') // no filename in arguments
		{
			fromstdin = 1;
			return mainFunc(argv[argc-1], NULL);
		}
		else
		{
			return mainFunc(argv[argc-2], argv[argc-1]);
		}
	}
	

	return 1;
}
