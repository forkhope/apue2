CC = gcc
CFLAGS = -Wall -o

.PHONY: all do

all: cli srv do

cli: cli.c
	$(CC) $(CFLAGS) $@ $^

srv: srv.c
	$(CC) $(CFLAGS) $@ $^

do:
	echo -e '\033[31mRUN THE PROGRAM!!!\033[0m'
	./srv &
	./cli foo.socket
