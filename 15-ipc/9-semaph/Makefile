CC = gcc
CFLAGS = -Wall
LFLAGS = -Wall -o
OBJS = $(patsubst %.c,%.o,$(wildcard *.c))
BIN = semaph

.PHONY: all clean do

all: $(BIN) clean do

$(BIN): $(OBJS)
	$(CC) $(LFLAGS) $@ $^

clean:
	rm -rf $(OBJS)

do:
	echo -e '\033[31mRUN THE PROGRAM!!!\033[0m'
	./semaph
	ipcs -s
