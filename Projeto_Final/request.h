#ifndef REQUEST_H
#define REQUEST_H

#include "macros.h"


typedef struct{
  int clientPID;
  int nSeats;
  int seatNumSize;
  int seatNum[MAX_CLI_SEATS];
} Request;

Request* NewRequest(int clientPID, int nSeats, int *seatNum, int seatNumSize);


#endif