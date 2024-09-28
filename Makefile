CC = clang
CFLAGS = -O1 -Wall -Wextra -pthread -std=c11

all: gettimings

gettimings: gettimings.o
	$(CC) $(CFLAGS) -o gettimings gettimings.o

gettimings.o: gettimings.c
	$(CC) $(CFLAGS) -c gettimings.c

clean:
	rm -f gettimings gettimings.o