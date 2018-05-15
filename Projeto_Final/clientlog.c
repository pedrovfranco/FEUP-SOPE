#include "clientlog.h"

#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include "macros.h"

void openLog()
{
	logFile = fopen(logFilename, "a");
	bookFile = fopen(bookFilename, "a");
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

void logAnswer(int * arr, int pid, int count)
{
	char log[1000] = "", book[1000] = "";
	//sprintf(log, "%.5d ", pid);
    if (arr[0] > 0)
    {
        for (int i = 1; i < arr[0] + 1; i++)
        {
            sprintf(log, "%s %i %i | %i  %.4d\n",log, pid, i, arr[0], arr[i]);
            sprintf(book,"%s %.4d\n", book, arr[i]);
        }
    }
    else{
        if (arr[0] == -1)
        {
            sprintf(log, "%s MAX", log);
        }
        else if (arr[0] == -2)
        {
            sprintf(log, "%s NST", log);
        }
        else if (arr[0] == -3)
        {
            sprintf(log, "%s IID", log);
        }
        else if (arr[0] == -4)
        {
            sprintf(log, "%s ERR", log);
        }
        else if (arr[0] == -5)
        {
            sprintf(log, "%s NAV", log);
        }
        else if (arr[0] == -6)
        {
            sprintf(log, "%s FUL", log);
        }
    }

    bookPrint(book);
	logPrint(log);
}