#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#define BUFFER_SIZE 1024

int options[6] = {0,0,0,0,0,0}; // i, l, n, c, w, r


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


const char* strContains(const char* str1, const char* str2) // Searches for str2 in str1
{
	int found;

	for (int i = 0; i < strlen(str1); ++i)
	{
		found = 1;

		for (int j = 0; j < strlen(str2); ++j)
		{
			if (options[0]) // -i
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
			if (options[4]) // -w
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


int fromFile(const char* pattern, const char* filename)
{
	FILE *file = fopen(filename, "r");

	if (file == NULL)
	{
		printf("File does not exist!\n");
		return 1;
	}

	char *buffer = NULL;
	size_t bufferSize = BUFFER_SIZE;

	unsigned int iteCounter = 0;
	unsigned int matchCounter = 0;
	int printedLastline;

	while(!feof(file))
	{
		getline(&buffer, &bufferSize, file);

		if (buffer[strlen(buffer)-1] != '\n')
					strcat(buffer, "\n");

		if (strContains(buffer, pattern) != NULL)
		{
			matchCounter++;

			if (options[1]) // -l
			{
				printf("%s\n", filename);

				fclose(file);
				return 0;
			}
			else
			{	
				if (options[2]) // -n
				{	
					printf("%u:", iteCounter+1);
				}
				
				printf("%s", buffer);
			}
		}

		iteCounter++;
	}

	fclose(file);

	if (options[3]) // -c
	{
		printf("%u\n", matchCounter);
	}
	// else
	// {
	// 	if (matchCounter != 0 || feof(file)) // Doesnt print a newline if there are no matches but does
	// 		printf("\n");
	// }

	return 0;
}

int errorMessage()
{
	printf("Usage: simgrep [OPTION] PATTERN [FILE]\n");
	return 1;
}


int main(int argc, char const *argv[])
{

	if (argc < 3)
	{
		return errorMessage();
	}

	for (int i = 1; i < argc - 2; ++i) // Fills options array. number of options = argc - 3
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
		else
		{
			return errorMessage();
		}
	}

	return fromFile(argv[argc-2], argv[argc-1]);
}
