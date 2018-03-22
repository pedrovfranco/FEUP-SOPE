cc		 = gcc
CFLAGS	 = -Wall -g
LDFLAGS  =
DEPS	 = simgrep.c aux_fun.c aux_fun.h
OBJFILES = aux_fun.o simgrep.o
TARGET	 = simgrep

all: $(TARGET)

$(TARGET): $(OBJFILES) $(DEPS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJFILES) $(LDFLAGS)

clean:
	rm -f $(OBJFILES) $(TARGET) *~