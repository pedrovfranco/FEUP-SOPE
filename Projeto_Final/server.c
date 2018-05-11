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
#include "requests.h"

int NUM_ROOM_SEATS; // Nmr de lugares disponiveis
int NUM_TICKET_OFFICES; // Nmr de bilheteiras
int OPEN_TIME = 0; // Tempo de funcionamento das bilheteiras

int REQUESTS_FIFO_FD; // File descriptor do fifo de requests
char * REQUESTS_FIFO_PATH = "/tmp/requests"; // Path do fifo requests

typedef struct{
  int seatId;
  int clientPID;
  int isTaken; // 0 esta livre 1 esta ocupado
}Seat;


Request* buffer[99];

int isSeatFree(Seat *seats, int seatNum) // 0 esta livre 1 ocupado 2 n existe esse lugar
{
  for (int i = 0; i < NUM_ROOM_SEATS; i++)
  {
    if (seats->seatId == seatNum)
    {
      if (seats->isTaken == 0)
      {
        return 0;
      }
      else 
        return 1;
    }
    //seats += sizeof(Seat);
    seats++;
  }
  return 2;
}

void bookSeat(Seat *seats, int seatNum, int clientId){
  for (int i = 0; i < NUM_ROOM_SEATS; i++)
  {
    if (seats->seatId == seatNum)
    {
      seats->isTaken = 1;
      seats->clientPID = clientId;
    }
    //seats += sizeof(Seat);
    seats++;
  }
}

void freeSeat(Seat *seats, int seatNum){
    for (int i = 0; i < NUM_ROOM_SEATS; i++)
  {
    if (seats->seatId == seatNum)
    {
      seats->isTaken = 0;
      seats->clientPID = 0;
    }
    //seats += sizeof(Seat);
    seats++;
  }
}

void * requestGenerator(void * arg)
{
  // Esta inicializado a 0 mas n e para o ser
  Request *request = malloc(sizeof(Request));
  request->clientPID = 0; // Para mudar
  request->nSeats = 0;
  request->seats = NULL;

  write(REQUESTS_FIFO_FD, request, sizeof(Request));
  pthread_exit(NULL);
}

void * listenRequests(void * arg)
{
  Request * request = malloc(sizeof(Request));
  int bytes;

  while (1)
  {
    usleep(1000*10); //10 milisegundos

    if ( (bytes = read(REQUESTS_FIFO_FD, request, sizeof(Request))) > 0)
    {
      printf("Gottim!\n");



    }

    // Colocar o request num buffer de requests
    // As threads bilheteira devem pegar no request e tentam reservar os lugares desse pedido
  }
}

void openRequestsFIFO()
{
	if ( (REQUESTS_FIFO_FD = open(REQUESTS_FIFO_PATH, O_RDONLY)) == -1 ) {
		perror("Failed to open REQUESTS_FIFO");
		exit(1);
	}
}

void makeRequestsFIFO()
{
	if (mkfifo(REQUESTS_FIFO_PATH, 0660) < 0) {
		if (errno != EEXIST) {
			perror("Error creating FIFO");
			exit(1);
		}
	}
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
  makeRequestsFIFO();

  // Abertura do fifo de requests
  // TODO: Abrir os fifos que vem dos clientes
  openRequestsFIFO();

  // Threads auxiliares - tentam reservar os lugares
  pthread_t tid1;
  if (pthread_create(&tid1, NULL, requestGenerator, NULL) != 0){
        printf("Error creating ticket offices threads");
        exit(1);
  }

  // Main Thread - recebe os requests(FIFO) e coloca os num buffer para serem recolhidos pelas threads bilheteira
  pthread_t tid2;
  if (pthread_create(&tid2, NULL, listenRequests, NULL) != 0){
        printf("Error creating main thread");
        exit(1);
  }

  pthread_join(tid1, NULL);
  pthread_join(tid2, NULL);

  //Close file descriptors
  close(REQUESTS_FIFO_FD);

  // Delete FIFOS
  unlink(REQUESTS_FIFO_PATH);


  // Fica a faltar a parte de escrever nos ficheiros e da sincronizacao das threads

  return 0;
}
