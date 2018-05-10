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

int NUM_ROOM_SEATS; // Nmr de lugares disponiveis
int NUM_TICKET_OFFICES; // Nmr de bilheteiras
int OPEN_TIME = 0; // Tempo de funcionamento das bilheteiras

int REQUESTS_FIFO_FD; // File descriptor do fifo de requests
char * REQUESTS_FIFO_PATH; // Path do fifo requests


typedef struct{
  int clientPID;
  int nSeats;
  int seats[];
} Request;

void * requestGenerator(void * arg)
{

}

void * listenRequests(void * arg)
{

}

void openRequestsFIFO()
{
	if ( (REQUESTS_FIFO_FD = open(REQUESTS_FIFO_PATH, O_WRONLY)) == -1 ) {
		perror("Failed to open REQUESTS_FIFO");
		exit(1);
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

  // Abertura do fifo de requests (falta meter o path)
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
