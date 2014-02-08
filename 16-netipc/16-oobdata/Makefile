CC = gcc
CFLAGS = -Wall -o

.PHONY: all do

all: oobdata oobsrv do

oobdata: oobdata.c
	$(CC) $(CFLAGS) $@ $^

oobsrv: oobsrv.c
	$(CC) $(CFLAGS) $@ $^

do:
	echo -e '\033[31mRUN THE PROGRAM!!!\033[0m'
	./oobsrv
