#ifndef LOG_H
#define LOG_H

#include <stdio.h>

#include "request.h"

static char *logFilename = "slog.txt";
static FILE* logFile;
static char* logString;

static char *bookFilename = "sbook.txt";
static FILE* bookFile;
static char* bookString;

void openLog();

void logPrint(char* str);

void bookPrint(char* str);

void logOpenTicketBooth(int boothId);

void logCloseTicketBooth(int boothId);

void logRequest(Request* request, int boothId, int answer, int *reservedSeats, int reservedSeatsSize);

void logCloseServer();

#endif