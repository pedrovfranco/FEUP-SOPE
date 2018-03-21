#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#define BUFFER_SIZE 128



int noOptions(int argc, char const *argv[]) // 
{
	FILE *file = fopen(argv[2], "r");

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
		
		if (strstr(buffer, argv[1]) != NULL)
		{
			printf("%s", buffer);
		}

	}

	fclose(file);

	return 0;

}


int main(int argc, char const *argv[])
{

	if (argc == 3)
	{
		return noOptions(argc, argv);
	}
	else
	{
		printf("Wrong number of arguments!\n");
		return 1;
	}

	return 1;
}
