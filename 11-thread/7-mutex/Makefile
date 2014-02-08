CC = gcc
CFLAGS = -Wall
OBJS = $(patsubst %.c,%.o,$(wildcard *.c))
BIN = mutex

all: $(BIN) clean do

$(BIN): $(OBJS)
	$(CC) -Wall -pthread -o $@ $^

clean:
	rm -f $(OBJS)

do:
	echo -e '\033[031mRUN THE PROGRAM!!!\033[0m'
	./mutex
