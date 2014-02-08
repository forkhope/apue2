CC = gcc
CFLAGS = -Wall
LFLAGS = -Wall -o
OBJS = $(patsubst %.c,%.o,$(wildcard *.c))
BIN = nonblock

.PHONY: all clean do

all: $(BIN) clean do

$(BIN): $(OBJS)
	$(CC) $(LFLAGS) $@ $^

clean:
	rm -f $(OBJS)

do:
	echo -e '\033[31mRUN THE PROGRAM!!!\033[0m'
	echo -e '\033[31mWRITE TO THE REGULAR FILE!!!\033[0m'
	./nonblock < inputfile > temp.file
	echo -e '\033[31mWRITE TO THE TERMINAL!!!\033[0m'
	./nonblock < inputfile 2> stderr.out
