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

void sendRequest()
{
  Request *request = malloc(sizeof(Request));
  request->clientPID = 5;
  request->seatNum = 3;

  write(REQUESTS_FIFO_FD, request, sizeof(Request));
}

int main(int argc, char *argv[]) {
  printf("** Running process %d (PGID %d) **\n", getpid(), getpgrp());

  if (argc == 4)
    printf("ARGS: %s | %s | %s\n", argv[1], argv[2], argv[3]);

  ANSWERS_FIFO_PATH = malloc(100);
  sprintf(ANSWERS_FIFO_PATH, "%s%u", "/tmp/ans", getpid());

  makeAnswerFIFO();
  openRequestsFIFO();
  sendRequest();


  close(REQUESTS_FIFO_FD);
  close(ANSWERS_FIFO_FD);

  unlink(ANSWERS_FIFO_PATH);
  return 0;
}