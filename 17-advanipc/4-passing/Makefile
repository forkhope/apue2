CC = gcc
CFLAGS = -Wall -o

.PHONY: all do

all: recv_fd send_fd do

recv_fd: recv_fd.c
	$(CC) $(CFLAGS) $@ $^

send_fd: send_fd.c
	$(CC) $(CFLAGS) $@ $^

do:
	echo -e '\033[31mRUN THE PROGRAM!!!\033[0m'
	./send_fd
