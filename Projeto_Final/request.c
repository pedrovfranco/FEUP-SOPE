#include "request.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Request* NewRequest(int clientPID, int nSeats, int *seatNum, int seatNumSize)
{
	Request* ret = malloc(sizeof(Request));

	ret->clientPID = clientPID;
	ret->nSeats = nSeats;
	ret->seatNumSize = seatNumSize;
	memcpy(ret->seatNum, seatNum, sizeof(int)*seatNumSize);
	
	return ret;
}