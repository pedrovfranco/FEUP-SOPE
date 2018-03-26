cc		 = gcc
CFLAGS	 = -Wall -Wextra -Werror -g
LDFLAGS  =
DEPS	 = simgrep.c aux_fun.c aux_fun.h log.c log.h makefile
OBJFILES = aux_fun.o simgrep.o log.o
TARGET	 = simgrep

all: $(TARGET)

$(TARGET): $(OBJFILES) $(DEPS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJFILES) $(LDFLAGS)

clean:
	rm -f $(OBJFILES) $(TARGET) *~
