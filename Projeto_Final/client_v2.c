#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>

#include "request.h"

//Macros
#define WIDTH_PID 5
#define WIDTH_XXNN 5
#define WIDTH_SEAT 4


int *pref_seat_list;
int pref_seat_list_size;

int ANSWERS_FIFO_FD; // File descriptor do fifo de requests
int REQUESTS_FIFO_FD;
char * ANSWERS_FIFO_PATH; // Path do fifo requests
char * REQUESTS_FIFO_PATH = "/tmp/requests"; // Path do fifo requests


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
	if ( (REQUESTS_FIFO_FD = open(REQUESTS_FIFO_PATH, O_RDONLY)) == -1 ) {
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

void sendRequest(int pid, int time_out, int num_wanted_seats)
{
	Request *request = NewRequest(pid, num_wanted_seats, pref_seat_list, pref_seat_list_size);

	write(REQUESTS_FIFO_FD, request, sizeof(Request));
}

int main(int argc, char *argv[]) {

	ANSWERS_FIFO_PATH = malloc(100);
	sprintf(ANSWERS_FIFO_PATH, "%s%u", "/tmp/ans", getpid());

	pref_seat_list_size = argc - 3;
	pref_seat_list = malloc(sizeof(int)*pref_seat_list_size);

// Leitura dos lugares para o array
	int i;
	for (i = 0; i < pref_seat_list_size; ++i)
	{
		pref_seat_list[i-3] = atoi(argv[i+3]);
	}

	for (int i = 0; i < pref_seat_list_size; i++)
	{
		printf("%d", pref_seat_list[i]);
	}

	makeAnswerFIFO();
	openRequestsFIFO();
	sendRequest(getpid(), atoi(argv[1]), atoi(argv[2]));


	close(REQUESTS_FIFO_FD);
	close(ANSWERS_FIFO_FD);

	unlink(ANSWERS_FIFO_PATH);
	return 0;
}