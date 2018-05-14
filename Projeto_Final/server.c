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
#include <signal.h>

#include "request.h"
#include "queue.h"
#include "macros.h"
#include "serverlog.h"

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int NUM_ROOM_SEATS; // Nmr de lugares disponiveis
int NUM_TICKET_OFFICES; // Nmr de bilheteiras
int OPEN_TIME = 0; // Tempo de funcionamento das bilheteiras

int REQUESTS_FIFO_FD; // File descriptor do fifo de requests
char * REQUESTS_FIFO_PATH = "/tmp/requests"; // Path do fifo requests

queue* requestBuffer;

pid_t mainPID;

pthread_t *threads;

typedef struct{
	int clientPID; //-1 se for um lugar vago
} Seat;

Seat *seats;

int closedPipe = 0, terminate = 0;

void sigpipe_handler(int signal)
{
	UNUSED(signal);
	closedPipe = 1;
}

void sigalrm_handler(int signal)
{
	UNUSED(signal);
	kill(0, SIGINT);
}

void sigint_handler(int signal) //Correct way to terminate the program
{
	UNUSED(signal);
	terminate = 1;
}


int hasInvalidSeat(int* arr, int arrSize) // Returns 1 if true and 0 otherwise
{
	for (int i = 0; i < arrSize; ++i)
	{
		if (arr[i] < 1 || arr[i] > NUM_ROOM_SEATS)
			return 1;
	}

	return 0;
}


int openWriteFIFO(char* filename)
{
	int fd;

	if ( (fd = open(filename, O_WRONLY)) == -1 ) {
		perror("Failed to open REQUESTS_FIFO");
		exit(1);
	}

	return fd;
}

int openReadFIFO(char* filename)
{
	int fd;

	if ( (fd = open(filename, O_RDONLY)) == -1 ) {
		// perror("Failed to open REQUESTS_FIFO");
		// exit(1);
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

int isSeatFree(Seat *s, int seatNum) // 0 esta livre 1 ocupado 2 n existe esse lugar
{
	if (seatNum > NUM_ROOM_SEATS || seatNum < 1)
		return 2;
	else if (s[seatNum-1].clientPID == -1)
		return 0;
	else
		return 1;
}

void bookSeat(Seat *s, int seatNum, int clientId)
{
	DELAY();
	s[seatNum-1].clientPID = clientId;	
}

void freeSeat(Seat *s, int seatNum)
{
	DELAY();
	s[seatNum-1].clientPID = -1;
}

int isRoomFull(Seat* s)
{
	DELAY();

	for (int i = 0; i < NUM_ROOM_SEATS; ++i)
	{
		if (isSeatFree(s, i) == 0)
			return 0;
	}

	return 1;
}

int pickAnswer(Seat* seats, Request* request)
{
	if (request->nSeats > MAX_CLI_SEATS)
	{
		return -1;
	}
	else if (request->seatNumSize < request->nSeats || request->seatNumSize > MAX_CLI_SEATS)
	{
		return -2;
	}
	else if (hasInvalidSeat(request->seatNum, request->seatNumSize))
	{
		return -3;
	}
	else if (isRoomFull(seats))
	{
		return -6;
	}

	return -7;
}


void * listenRequests(void * arg)
{
	UNUSED(arg);

	mainPID = getpid();

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

		if (terminate)
		{
			printf("Caught\n");

			pthread_exit(NULL);
		}
	}

	pthread_exit(NULL);
}

void * handleRequests(void * arg)
{
	struct sigaction intaction;

	intaction.sa_handler = sigint_handler;
	sigemptyset(&intaction.sa_mask);
	intaction.sa_flags = 0;

	if (sigaction(SIGINT, &intaction, NULL) != 0)
	{
		perror("Unable to install SIGINT handler\n");
		pthread_exit(NULL);
	}

	logOpenTicketBooth(*(int*)arg);

	int client_fifo_pd;
	char * client_fifo_path = malloc(100);
	Request* request;
	int answer;
	int *reservedSeats;
	int reservedSeatsSize = 0;

	while (1)
	{
		usleep(1000*10); //Sleep for 10 milliseconds

		pthread_mutex_lock(&mutex);

		if (!qisEmpty(requestBuffer))
		{
			answer = 1337;
			printf("Thread nr %u handled this request\n", *(int*)(arg));

			request = qremoveData(requestBuffer);

			answer = pickAnswer(seats, request);
			reservedSeats = malloc(sizeof(int)*request->nSeats);
			reservedSeatsSize = 0;

			client_fifo_path[0] = 0;
			sprintf(client_fifo_path, "%s%u", "/tmp/ans", request->clientPID);

			client_fifo_pd = openWriteFIFO(client_fifo_path);

			if (answer == -7)
			{
				answer = 0;

				int i;
				for (i = 0; i < request->seatNumSize && reservedSeatsSize < request->nSeats; i++)
				{
					if (isSeatFree(seats, request->seatNum[i]) == 0)
					{
						reservedSeats[reservedSeatsSize] = request->seatNum[i];
						reservedSeatsSize++;
						bookSeat(seats, request->seatNum[i], request->clientPID);
					}
				}

				if (reservedSeatsSize < request->nSeats) //Failed
				{
					for (i = 0; i < reservedSeatsSize; ++i) //Frees reserved seats
						freeSeat(seats, reservedSeats[i]);

					answer = -5;
				}	
			}

			// sleep(3);

			if (answer == 0) //Sucess
			{
				if (!closedPipe)
					write(client_fifo_pd, &reservedSeatsSize, sizeof(reservedSeatsSize));

				for (int i = 0; i < reservedSeatsSize; ++i)
				{
					if (!closedPipe)
						write(client_fifo_pd, &reservedSeats[i], sizeof(reservedSeats[i]));
				}
			}
			else
			{
				if (!closedPipe)
					write(client_fifo_pd, &answer, sizeof(answer));
			}

			close(client_fifo_pd);

			logRequest(request, *(int*)arg, answer, reservedSeats, reservedSeatsSize);

		}

		pthread_mutex_unlock(&mutex);
	
		if (terminate)
			pthread_exit(NULL);
	}

	
	pthread_exit(NULL);
}


int main(int argc, char *argv[]) {

	if (argc != 4) {
		printf("Usage: %s  <num_room_seats> <num_ticket_offices> <open_time>\n", argv[0]);
		exit(1);
	}
	printf("ARGS: %s | %s | %s\n", argv[1], argv[2], argv[3]);

  	// Inicializacao dos atributos
	NUM_ROOM_SEATS = atoi(argv[1]);
	NUM_TICKET_OFFICES = atoi(argv[2]);
	OPEN_TIME = atoi(argv[3]);

	struct sigaction pipeaction;

	pipeaction.sa_handler = sigpipe_handler;
	sigemptyset(&pipeaction.sa_mask);
	pipeaction.sa_flags = 0;

	if (sigaction(SIGPIPE, &pipeaction, NULL) != 0)
	{
		perror("Unable to install SIGPIPE handler\n");
		return 1;
	}

	struct sigaction alarmaction;

	alarmaction.sa_handler = sigalrm_handler;
	sigemptyset(&alarmaction.sa_mask);
	alarmaction.sa_flags = 0;

	if (sigaction(SIGALRM, &alarmaction, NULL) != 0)
	{
		perror("Unable to install SIGALRM handler\n");
		return 1;
	}

	struct sigaction intaction;

	intaction.sa_handler = sigint_handler;
	sigemptyset(&intaction.sa_mask);
	intaction.sa_flags = 0;

	if (sigaction(SIGINT, &intaction, NULL) != 0)
	{
		perror("Unable to install SIGINT handler\n");
		return 1;
	}

	alarm(OPEN_TIME);

	openLog();

  	//Criacao do fifo de requests
	createFIFO(REQUESTS_FIFO_PATH);

  	// Abertura do fifo de requests
  	// TODO: Abrir os fifos que vem dos clientes
	REQUESTS_FIFO_FD = openReadFIFO(REQUESTS_FIFO_PATH);

	requestBuffer = qcreate(1); //Creates queue of Resquest pointers with a maximum size of 1

	seats = malloc(sizeof(Seat)*NUM_ROOM_SEATS);
	for (int i = 0; i < NUM_ROOM_SEATS; ++i)
	{
		seats[i].clientPID = -1;
	}

  	// Mutex
  	pthread_mutex_init(&mutex, NULL);


  	// Main Thread - recebe os requests(FIFO) e coloca os num buffer para serem recolhidos pelas threads bilheteira
	pthread_t tid1;
	if (pthread_create(&tid1, NULL, listenRequests, NULL) != 0){
		printf("Error creating main thread");
		exit(1);
	}



  	// Bilheteira thread - 
  	threads = malloc(sizeof(pthread_t)*NUM_TICKET_OFFICES);
	int* ponteiro;
	for (int i = 0;i < NUM_TICKET_OFFICES; i++)
	{
		ponteiro = malloc(sizeof(int));
		*ponteiro = i+1;
		if (pthread_create(&threads[i], NULL, handleRequests, ponteiro) != 0){
			printf("Error creating ticket booth thread");
			exit(1);
		}
	}
	pthread_join(tid1, NULL);

	for (int i = 0;i < NUM_TICKET_OFFICES; i++)
	{
		pthread_join(threads[i], NULL);
		logCloseTicketBooth(i+1);
	}

 	 //Close mutex 
  	pthread_mutex_destroy(&mutex);

 	 //Close file descriptors
	close(REQUESTS_FIFO_FD);

 	 // Delete FIFOS
	unlink(REQUESTS_FIFO_PATH);

	logCloseServer();


  	// Fica a faltar a parte de escrever nos ficheiros e da sincronizacao das threads

	return 0;
}
