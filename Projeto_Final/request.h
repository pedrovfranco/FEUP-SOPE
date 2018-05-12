#ifndef REQUEST_H
#define REQUEST_H

//#include ""
#define MAX_CLI_SEATS 99


typedef struct{
  int clientPID;
  int nSeats;
  int seatNum[MAX_CLI_SEATS];
} Request;


#endif