#include "serverlog.h"

#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include "macros.h"

void openLog()
{
	logFile = fopen(logFilename, "w");
	bookFile = fopen(bookFilename, "w");
}


void logPrint(char* str)
{
	fprintf(logFile, "%s\n", str);
	fflush(logFile);
}

void bookPrint(char* str)
{
	fprintf(bookFile, "%s", str);
	fflush(bookFile);
}

void logOpenTicketBooth(int boothId)
{
	char str[50];
	sprintf(str, "%u%u-OPEN", (boothId/10)%10, boothId%10);
	logPrint(str);
}

void logCloseTicketBooth(int boothId)
{
	char str[50];
	sprintf(str, "%u%u-CLOSED", (boothId/10)%10, boothId%10);
	logPrint(str);
}

void logRequest(Request* request, int boothId, int answer, int *reservedSeats, int reservedSeatsSize)
{
	char log[1000] = "", book[1000] = "";
	sprintf(log, "%.2d-%.5d-%.2d:", boothId, request->clientPID, request->nSeats);

	for (int i = 0; i < request->seatNumSize; ++i)
	{
		sprintf(log, "%s %.4d", log, request->seatNum[i]);
	}

	sprintf(log, "%s\t-", log);

	char foo[100] = "";
	sprintf(foo, "%s%s%u%s", "%s", "%.", WIDTH_SEAT, "u\n");

	if (answer == 0) //Sucess, print reservedSeats
	{
		for (int i = 0; i < reservedSeatsSize; ++i)
		{
			sprintf(log, "%s %.4u", log, reservedSeats[i]);
			sprintf(book, foo, book, reservedSeats[i]);
		}
	}
	else if (answer == -1)
	{
		sprintf(log, "%s MAX", log);
	}
	else if (answer == -2)
	{
		sprintf(log, "%s NST", log);
	}
	else if (answer == -3)
	{
		sprintf(log, "%s IID", log);
	}
	else if (answer == -4)
	{
		sprintf(log, "%s ERR", log);
	}
	else if (answer == -5)
	{
		sprintf(log, "%s NAV", log);
	}
	else if (answer == -6)
	{
		sprintf(log, "%s FUL", log);
	}
	
	logPrint(log);
	bookPrint(book);
}

void logCloseServer()
{
	logPrint("SERVER CLOSED");
}