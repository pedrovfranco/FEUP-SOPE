#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

int isWordCharacter(const char ch);

int intlength(int input);

int cmpi(char ch1, char ch2);

int errorMessage();