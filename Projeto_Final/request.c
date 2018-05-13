#include "request.h"

#include <stdio.h>
#include <stdlib.h>

Request* NewRequest(int clientPID, int nSeats, int *seatNum, int seatNumSize)
{
	Request* ret = malloc(sizeof(Request));

	ret->clientPID = clientPID;
	ret->nSeats = nSeats;
	ret->seatNum = seatNum;
	ret->seatNumSize = seatNumSize;

	return ret;
}