#include <stdio.h>

#include <sys/types.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  
	if (argc != 4) {
		printf("Usage: %s  <num_room_seats> <num_ticket_offices> <open_time>\n", argv[0]);
		exit(0);
	}

  else
    printf("ARGS: %s | %s | %s\n", argv[1], argv[2], argv[3]);

  sleep(1);

  return 0;
}
