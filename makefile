cc		 = gcc
CFLAGS	 = -Wall
LDFLAGS  =
OBJFILES = aux_fun.o simgrep.o
TARGET	 = simgrep

all: $(TARGET)

$(TARGET): $(OBJFILES)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJFILES) $(LDFLAGS)

clean:
	rm -f $(OBJFILES) $(TARGET) *~