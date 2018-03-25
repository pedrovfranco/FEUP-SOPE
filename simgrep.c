#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <dirent.h>

#include "aux_fun.h"

#define BUFFER_SIZE 1024

int options[6] = {0,0,0,0,0,0}; // i, l, n, c, w, r
int fromstdin = 0; // Flag that determines whether the program is reading from a file or from the standard input

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

	struct stat st;

	if (stat(filename, &st) == 0 && S_ISDIR(st.st_mode))
	{
		printf("simgrep: %s: Is a directory\n", filename);
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

		if (strContains(buffer, pattern, options) != NULL)
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
				if (options[5]) // -r, prints the filename if recursive option is activated
					printf("%s:", filename+2);

				if (options[2]) // -n, iniciates the print with the line number.
					printf("%u:", iteCounter+1);

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

// Coisas

int recursiveFunc(const char* pattern, const char* dirname)
{
	DIR* currdr = opendir(dirname);
	struct dirent *curr;
	struct stat st;
	pid_t pid;
	char path[512]; // Path of the file
	char dir[512]; // path of the current directory
	strcpy(dir, dirname);

	while ((curr = readdir(currdr)) != NULL)
	{
		sprintf(path, "%s/%s", dir, curr->d_name);

		if (strcmp(curr->d_name, "..") != 0 && strcmp(curr->d_name, ".") != 0)
		{
			if (stat(path, &st) == 0)
			{
				if (S_ISDIR(st.st_mode)) // Is a directory
				{
					pid = fork();

					if (pid > 0) // Parent
					{
						wait(NULL);
					}
					else if (pid == 0) // Child
					{
						return recursiveFunc(pattern, path);
					}
					else
					{
						printf("Fork error\n");
						return 1;
					}
				}
				else // Is a regular file
				{
					if (mainFunc(pattern, path) != 0)
						return 1;
				}

			}
		}
	}

	closedir(currdr);

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
		if (argc == 2 || argv[argc-2][0] == '-') // no filename in arguments
		{
			return recursiveFunc(argv[argc-1], ".");
		}
		else
		{
			char path[128] = "./";
			strcat(path, argv[argc-1]);

			struct stat st;

			if (stat(path, &st) != 0)
			{
				printf("Error reading file!\n");
				return 1;
			}

			if (S_ISDIR(st.st_mode))
			{
				return recursiveFunc(argv[argc-2], path);
			}
			else // If the filename is not a directory -r is meaningless
			{
				options[5] = 0;
				return mainFunc(argv[argc-2], path);
			}
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
