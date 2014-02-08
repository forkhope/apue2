CC = gcc
CFLAGS = -Wall -o

all: fifo wfifo do

fifo: fifo.c
	$(CC) $(CFLAGS) $@ $^

wfifo: wfifo.c
	$(CC) $(CFLAGS) $@ $^

do:
	echo -e '\033[31mRUN THE PROGRAM!!!\033[0m'
	rm -f tempfifo
