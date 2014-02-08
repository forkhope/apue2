CC = gcc
CFLAGS = -Wall
LFLAGS = -Wall -o
OBJS = $(patsubst %.c,%.o,$(wildcard *.c))
BIN = pager

.PHONY: all clean do

all: $(BIN) clean do

$(BIN): $(OBJS)
	$(CC) $(LFLAGS) $@ $^

clean:
	rm -f $(OBJS)

do:
	echo -e '\033[33mRUN THE PROGRAM!!!\033[0m'
	./pager pager.c
