#ifndef REQUEST_H
#define REQUEST_H



typedef struct{
  int clientPID;
  int nSeats;
  int *seatNum;
  int seatNumSize;
} Request;

Request* NewRequest(int clientPID, int nSeats, int *seatNum, int seatNumSize);


#endif