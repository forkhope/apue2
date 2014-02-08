CC = gcc
CFLAGS = -Wall -o

all: msgsnd msgrcv do

msgsnd: msgsnd.c
	$(CC) $(CFLAGS) $@ $^

msgrcv: msgrcv.c
	$(CC) $(CFLAGS) $@ $^

do:
	echo -e '\033[31mRUN THE PROGRAM!!!\033[0m'
	./msgsnd
	ipcs -q
	./msgrcv
	ipcs -q
