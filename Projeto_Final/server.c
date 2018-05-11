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

#include "queue.h"
#include "request.h"

int NUM_ROOM_SEATS; // Nmr de lugares disponiveis
int NUM_TICKET_OFFICES; // Nmr de bilheteiras
int OPEN_TIME = 0; // Tempo de funcionamento das bilheteiras

int REQUESTS_FIFO_FD; // File descriptor do fifo de requests
char * REQUESTS_FIFO_PATH = "/tmp/requests"; // Path do fifo requests

queue* requestBuffer;


typedef struct{
	int clientPID; //-1 se for um lugar vago
} Seat;



int openFIFO(char* filename)
{
	int fd;

	if ( (fd = open(filename, O_RDONLY)) == -1 ) {
		printf("Hortalicas\n");
		perror("Failed to open REQUESTS_FIFO");
		exit(1);
	}

	return fd;
}

void createFIFO(char* filename)
{
	if (mkfifo(filename, 0660) < 0) {
		if (errno != EEXIST) {
			perror("Error creating FIFO");
			exit(1);
		}
	}
}

int isSeatFree(Seat *seats, int seatNum) // 0 esta livre 1 ocupado 2 n existe esse lugar
{
	if (seatNum > NUM_ROOM_SEATS || seatNum < 1)
		return 2;
	else if (seats[seatNum-1].clientPID == -1)
		return 0;
	else
		return 1;
}

void bookSeat(Seat *seats, int seatNum, int clientId)
{
	seats[seatNum-1].clientPID = clientId;	
}

void freeSeat(Seat *seats, int seatNum)
{
	seats[seatNum-1].clientPID = -1;
}


void * listenRequests(void * arg)
{
	Request * request;
	Request * auxiliaryRequest = malloc(sizeof(Request));
	int bytes;

	while (1)
	{
		usleep(1000*10); //10 milisegundos

	 if ( (bytes = read(REQUESTS_FIFO_FD, auxiliaryRequest, sizeof(Request))) > 0)
	 {
		printf("Listened\n");

		request = malloc(sizeof(Request));
		*request = *auxiliaryRequest;
		qinsert(requestBuffer, request);
	 }

	 // Colocar o request num buffer de requests
	 // As threads bilheteira devem pegar no request e tentam reservar os lugares desse pedido
	}

	pthread_exit(NULL);
}

void * handleRequests(void * arg)
{
	Seat* seats = (Seat*) arg;
	int client_fifo_pd;
	char * client_fifo_path;
	Request* request;

	while (1)
	{
		usleep(1000*10); //Sleep for 10 milliseconds

		if (!qisEmpty(requestBuffer))
		{
			printf("Handled\n");
			request = qremoveData(requestBuffer);

			printf("request.clientPID = %d\nrequest.seatNum = %d\n", request->clientPID, request->seatNum);

			if (isSeatFree(seats, request->seatNum) == 0)
			{
				printf("Livre\n");
				bookSeat(seats, request->seatNum, request->clientPID);
			}
			else
			{
				printf("Ocupado\n");
			}
		}
	}


	pthread_exit(NULL);
}


int main(int argc, char *argv[]) {

	if (argc != 4) {
		printf("Usage: %s  <num_room_seats> <num_ticket_offices> <open_time>\n", argv[0]);
		exit(0);
	}
	printf("ARGS: %s | %s | %s\n", argv[1], argv[2], argv[3]);

  // Inicializacao dos atributos
	NUM_ROOM_SEATS = atoi(argv[1]);
	NUM_TICKET_OFFICES = atoi(argv[2]);
	OPEN_TIME = atoi(argv[3]);

  //Criacao do fifo de requests
	createFIFO(REQUESTS_FIFO_PATH);

  // Abertura do fifo de requests
  // TODO: Abrir os fifos que vem dos clientes
	REQUESTS_FIFO_FD = openFIFO(REQUESTS_FIFO_PATH);

	requestBuffer = qcreate(99); //Creates queue of Resquest pointers with a maximum size of 99

	Seat seats[NUM_ROOM_SEATS];
	for (int i = 0; i < NUM_ROOM_SEATS; ++i)
	{
		seats[i].clientPID = -1;
	}

  // Main Thread - recebe os requests(FIFO) e coloca os num buffer para serem recolhidos pelas threads bilheteira
  pthread_t tid1;
  if (pthread_create(&tid1, NULL, listenRequests, NULL) != 0){
	printf("Error creating main thread");
	exit(1);
  }

  // Bilheteira thread - 
  pthread_t tid2;
  if (pthread_create(&tid2, NULL, handleRequests, seats) != 0){
		  printf("Error creating ticket booth thread");
		  exit(1);
  }


  // pthread_join(tid1, NULL);
  pthread_join(tid1, NULL);
  pthread_join(tid2, NULL);

  //Close file descriptors
  close(REQUESTS_FIFO_FD);

  // Delete FIFOS
  unlink(REQUESTS_FIFO_PATH);


  // Fica a faltar a parte de escrever nos ficheiros e da sincronizacao das threads

  return 0;
}
