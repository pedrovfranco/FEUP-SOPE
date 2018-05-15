#ifndef CLI_H
#define CLI_H

#include <stdio.h>

#include "request.h"

static char *logFilename = "clog.txt";
static FILE* logFile;
static char* logString;

static char *bookFilename = "cbook.txt";
static FILE* bookFile;
static char* bookString;

void openLog();

void logPrint(char* str);

void bookPrint(char* str);

void logAnswer(int * arr, int pid, int count);

#endif