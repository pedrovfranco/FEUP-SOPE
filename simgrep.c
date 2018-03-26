#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <dirent.h>
#include <math.h>
#include <time.h>

#include "aux_fun.h"
#include "log.h"

#define BUFFER_SIZE 1024
#define PATH_SIZE 512

int options[6] = {0,0,0,0,0,0}; // i, l, n, c, w, r
int fromstdin = 0; // Flag that determines whether the program is reading from a file or from the standard input.
pid_t ppid = 0; // Pid of the parent process.
int continueFlag = 0; // Controls the flow of the child processes such that they must wait for the SIGUSR1 signal to continue.

void sigint_handler()
{
	logPrint("SINAL SIGINT");

	if (getpid() == ppid)
	{
		char buffer[16];

		printf("Are you sure you want to terminate the program? (Y/N) ");
		scanf("%s", buffer);

		if (strcmp(buffer, "Y") == 0 || strcmp(buffer, "y") == 0) // Yes
		{
			kill(-ppid, SIGKILL);
			exit(3);
		}
		else
		{
			kill(-ppid, SIGUSR1);
		}
	}
	else // Waits for the SIGUSR1 signal from the parent process to continue.
	{
		while (!continueFlag)
		{
			usleep(1000); // Sleeps for 1 millisecond.
		}
	}
}

void sigusr1_handler() // Changes status of continueFlag to true, i.e., allows to child processes to continue
{
	logPrint("SINAL SIGUSR1");

	continueFlag = 1;
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

	if (file != stdin)
	{
		char foobar[128];
		
		if (filename[0] == '.' && filename[1] == '/') // If there is "./" in the path then discard it
			sprintf(foobar, "ABERTO %s", filename+2);
		else
			sprintf(foobar, "ABERTO %s", filename);

		logPrint(foobar);
	}

	struct stat st;

	if (stat(filename, &st) == 0 && S_ISDIR(st.st_mode))
	{
		printf("simgrep: %s: Is a directory\n", filename);
		fclose(file);
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

			if (options[1]) // -l, prints only the filenames of files with matches to pattern.
			{
				printf("%s\n", filename+2);

				fclose(file);

				char foobar[128];

				if (filename[0] == '.' && filename[1] == '/') // If there is "./" in the path then discard it
					sprintf(foobar, "FECHADO %s", filename+2);
				else
					sprintf(foobar, "FECHADO %s", filename);

				logPrint(foobar);

				return 0;
			}
			else if (options[3])
			{
				matchCounter++;
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

	char foobar[128];

	if (filename[0] == '.' && filename[1] == '/') // If there is "./" in the path then discard it
		sprintf(foobar, "FECHADO %s", filename+2);
	else
		sprintf(foobar, "FECHADO %s", filename);

	logPrint(foobar);

	if (options[3]) // -c, prints the number of matches.
	{
		if (options[5]) // -r, prints the filename if recursive option is activated
			printf("%s:", filename+2);

		printf("%u\n", matchCounter);
	}

	return 0;
}

int recursiveFunc(const char* pattern, const char* dirname)
{
	DIR* currdr = opendir(dirname);

	if (currdr == NULL)
	{
		printf("Error opening folder!\n");
		return 1;
	}

	struct dirent *curr;
	struct stat st;
	pid_t pid;
	char path[PATH_SIZE]; // Path of the file

	while ((curr = readdir(currdr)) != NULL)
	{
		sprintf(path, "%s/%s", dirname, curr->d_name);

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

int main(int argc, char *argv[])
{
	if (argc < 2)
	{
		return errorMessage();
	}

	start(getenv("LOGFILENAME"));

	char cmd[128] = "";
	int i;
	for (i = 0; i < argc; ++i) // Generate command to output to log file
	{
		strcat(cmd, argv[i]);
		if (i != argc-1)
			strcat(cmd, " ");
	}

	logPrint(cmd);

	ppid = getpid();
 
	struct sigaction sigintaction, sigusr1action;

	sigintaction.sa_handler = sigint_handler;
	sigemptyset(&sigintaction.sa_mask);
	sigintaction.sa_flags = 0;

	if (sigaction(SIGINT, &sigintaction, NULL) < 0)
	{
		fprintf(stderr,"Unable to install SIGINT handler\n");
		return 1;
	}

	sigusr1action.sa_handler = sigusr1_handler;
	sigemptyset(&sigusr1action.sa_mask);
	sigusr1action.sa_flags = 0;

	if (sigaction(SIGUSR1, &sigusr1action, NULL) < 0)
	{
		fprintf(stderr,"Unable to install SIGUSR1 handler\n");
		return 1;
	}

	for (i = 1; i < argc - 1; ++i) // Fills options array. Starts at 1 because of 
	{
		if (argv[i][0] == '-')
		{
			if (strlen(argv[i]) == 2)
			{
				if (strcmp(argv[i], "-i") == 0) // Disregard case differences ('A' = 'a')
				{
					options[0] = 1;
				}
				else if (strcmp(argv[i], "-l") == 0) // Shows the filename of the files with at least on match 
				{
					options[1] = 1;
				}
				else if (strcmp(argv[i], "-n") == 0) // Also prints the line number of the matching line
				{
					options[2] = 1;
				}
				else if (strcmp(argv[i], "-c") == 0) // Prints the number of matches
				{
					options[3] = 1;
				}
				else if (strcmp(argv[i], "-w") == 0) // Check if the matching pattern is a word (see grep's definition of word)
				{
					options[4] = 1;
				}
				else if (strcmp(argv[i], "-r") == 0) // Applies recursively
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
			char path[PATH_SIZE] = "./";
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
