#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>
#include <errno.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>

#include "request.h"
#include "macros.h"
#include "clientlog.h"


int pref_seat_list[MAX_CLI_SEATS];
int pref_seat_list_size = 0;

int ANSWERS_FIFO_FD; // File descriptor do fifo de requests
int REQUESTS_FIFO_FD;
char ANSWERS_FIFO_PATH[100]; // Path do fifo requests
char * REQUESTS_FIFO_PATH = "/tmp/requests"; // Path do fifo requests

int answered = 0;


void sigalrm_handler(int signal)
{
	if (!answered)
	{
		printf("Time-out!\n");
		close(REQUESTS_FIFO_FD);
		close(ANSWERS_FIFO_FD);
		unlink(ANSWERS_FIFO_PATH);
		exit(2);
	}
}

void fillSeatsList(char* str)
{
	int begin = -1;
	int end;
	int counter = 0;
	char buffer[1000];

	unsigned int i;
	for (i = 0; i < strlen(str); ++i)
	{
		if (str[i] == ' ')
		{
			counter++;
		}
	}

	// pref_seat_list = malloc(sizeof(int) * (counter+1));

	for (i = 0; i < strlen(str); ++i)
	{
		if (str[i] == ' ')
		{
			end = i;

			memcpy(buffer, str+begin+1, end - begin - 1);
			buffer[end - begin - 1] = 0;

			pref_seat_list[pref_seat_list_size] = atoi(buffer);
			pref_seat_list_size++;

			begin = end;
		}
	}

	end = strlen(str);

	memcpy(buffer, str+begin+1, end - begin - 1);
	buffer[end - begin + 1] = 0;

	pref_seat_list[pref_seat_list_size] = atoi(buffer);
	pref_seat_list_size++;
}

void makeAnswerFIFO()
{
	if (mkfifo(ANSWERS_FIFO_PATH, 0660) < 0) {
		if (errno != EEXIST) {
			perror("Error creating FIFO");
			exit(1);
		}
	}
}

void openAnswerFIFO()
{
	if ( (ANSWERS_FIFO_FD = open(ANSWERS_FIFO_PATH, O_RDONLY)) == -1 ) {
		perror("Failed to open ANSWERS_FIFO");
		exit(1);
	}
}

void openRequestsFIFO()
{
	if ( (REQUESTS_FIFO_FD = open(REQUESTS_FIFO_PATH, O_WRONLY)) == -1 ) {
		perror("Failed to open REQUESTS_FIFO");
		exit(1);
	}
}

void sendRequest(int pid, int num_wanted_seats)
{
	Request *request = NewRequest(pid, num_wanted_seats, pref_seat_list, pref_seat_list_size);

	write(REQUESTS_FIFO_FD, request, sizeof(Request));
}

void printAnswer()
{
	int buffer;
	int flag = 0;
	int bytes;

	int count = 0;
	int arr[100];
	for (int i = 0; i < 100; i++)
	{
		arr[i] = -9;
	}

	while (1)
	{
		usleep(1000*10); //10 milisegundos
		bytes = read(ANSWERS_FIFO_FD, &buffer, sizeof(buffer));

		if (bytes > 0)
		{
			answered = 1;
			//printf("%i ", buffer);
			arr[count] = buffer;
			count++;
			flag = 1;
		}
		else
		{
			if (flag)
			{
				printf("fgag");
				logAnswer(arr, getpid(), count);
				for (int i = 0; i < 100; i++)
				{
					arr[i] = -9;
				}
				break;
			}
		}
	}
}


int main(int argc, char *argv[]) {

	if (argc != 4) {
		printf("Usage: %s <time_out> <num_wanted_seats> <pref_seat_list>\n", argv[0]);
		exit(1);
	}
	printf("ARGS: %s | %s | %s\n", argv[1], argv[2], argv[3]);

	struct sigaction alarmaction;

	alarmaction.sa_handler = sigalrm_handler;
	sigemptyset(&alarmaction.sa_mask);
	alarmaction.sa_flags = 0;

	if (sigaction(SIGALRM, &alarmaction, NULL) != 0)
	{
		perror("Unable to install SIGALRM handler\n");
		return 1;
	}

	sprintf(ANSWERS_FIFO_PATH, "%s%u", "/tmp/ans", getpid());

	fillSeatsList(argv[3]);

	openLog();

	makeAnswerFIFO();
	openRequestsFIFO();
	sendRequest(getpid(), atoi(argv[2]));
	openAnswerFIFO();

	alarm(1);
	printAnswer();

	close(REQUESTS_FIFO_FD);
	close(ANSWERS_FIFO_FD);

	unlink(ANSWERS_FIFO_PATH);
	return 0;
}