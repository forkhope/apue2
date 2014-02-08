CC = gcc
CFLAGS = -Wall -o

.PHONY: all do

all: client uptimesrv do

client: client.c
	$(CC) $(CFLAGS) $@ $^

uptimesrv: uptimesrv.c
	$(CC) $(CFLAGS) $@ $^

do:
	echo -e '\033[31mRUN THE PROGRAM!!!\033[0m'
	./uptimesrv
	./client god
