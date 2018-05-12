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

//Macros
#define MAX_CLI_SEATS 99
#define WIDTH_PID 5
#define WIDTH_XXNN 5
#define WIDTH_SEAT 4

typedef struct{
  int clientPID;
  int seatNum[MAX_CLI_SEATS];
} Request;

int clientPID, time_out, num_wanted_seats; 
int pref_seat_list[MAX_CLI_SEATS];

int ANSWERS_FIFO_FD; // File descriptor do fifo de requests
int REQUESTS_FIFO_FD;
char * ANSWERS_FIFO_PATH; // Path do fifo requests
char * REQUESTS_FIFO_PATH = "/tmp/requests"; // Path do fifo requests

void initializeArray()
{
  for (int i = 0; i < MAX_CLI_SEATS; i++)
  {
    pref_seat_list[i] = -1;
  }
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
  request->clientPID = clientPID;
  for (int i = 0; i < MAX_CLI_SEATS; i++)
  {
    request->seatNum[i] = pref_seat_list[i];
  }

  // time_t t;
  // srand((unsigned) time(&t));
  // request->seatNum = rand()%20;

  write(REQUESTS_FIFO_FD, request, sizeof(Request));
}

int main(int argc, char *argv[]) {

  printf("** Running process %d (PGID %d) **\n", getpid(), getpgrp());

  if (argc >= 4)
    printf("ARGS: %s | %s | %s\n", argv[1], argv[2], argv[3]);

  ANSWERS_FIFO_PATH = malloc(100);
  sprintf(ANSWERS_FIFO_PATH, "%s%u", "/tmp/ans", getpid());

// Inicializa o array de pref seats a -1
  initializeArray();

  clientPID = getpid();
  time_out = atoi(argv[1]);
  num_wanted_seats = atoi(argv[2]);

// Leitura dos lugares para o array
  int i = 3;
  while(argv[i] != NULL)
  {
    pref_seat_list[i-3] = atoi(argv[i]);
    i++;
  }

  for (int i = 0; i < MAX_CLI_SEATS; i++)
  {
    printf("%d", pref_seat_list[i]);
  }

  makeAnswerFIFO();
  openRequestsFIFO();
  sendRequest();


  close(REQUESTS_FIFO_FD);
  close(ANSWERS_FIFO_FD);

  unlink(ANSWERS_FIFO_PATH);
  return 0;
}