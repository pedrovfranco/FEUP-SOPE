#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#define BUFFER_SIZE 512


char* strContains(const char* str1, const char* str2, const int arguments[]) // Searches for str2 in str1
{
	int found = 1;

	int i;
	for (i = 0; i < strlen(str1); ++i)
	{
		found = 1;

		for (int j = 0; j < strlen(str1); ++j)
		{
			if (arguments[0]) // -i maiusculas
			{

			}
			else
			{
				if (str1[i+j] != str2[j])
				{
					found = 0;
					break;
				}
			}
		}

		if (found) // Found
			return str[i];
	}
}


int noOptions(const char* pattern, const char* filename, const int arguments[])
{
	FILE *file = fopen(filename, "r");

	if (file == NULL)
	{
		printf("File does not exist!\n");
		return 1;
	}

	char *buffer = NULL;
	int numRead = BUFFER_SIZE;
	size_t bufferSize = BUFFER_SIZE;

	while(!feof(file))
	{
		numRead = getline(&buffer, &bufferSize, file);
		
		if (strstr(buffer, pattern) != NULL)
		{
			printf("%s", buffer);
		}

	}

	fclose(file);

	return 0;

}


int main(int argc, char const *argv[])
{

	int arguments[6] = {0,0,0,0,0,0}; // 0 = i, 1 = l, 2 = n, 3 = c, 4 = w, 5 = r

	int i;
	for (i = 0; i < argc; ++i)
	{
		if (strcmp())
	}

	if (argc == 3)
	{
		return noOptions(argv[1], argv[2], arguments);
	}
	else
	{
		printf("Wrong number of arguments!\n");
		return 1;
	}

	return 1;
}
