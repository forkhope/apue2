CC = gcc
CFLAGS = -Wall
LFLAGS = -Wall -pthread -o
OBJS = $(patsubst %.c,%.o,$(wildcard *.c))
BIN = exitjoin

.PHONY: all clean do

all: $(BIN) clean do

$(BIN): $(OBJS)
	$(CC) $(LFLAGS) $@ $^

clean:
	rm -f $(OBJS)

do:
	echo -e '\033[31mEXECUTE THE BIN!!!\033[0m'
	./exitjoin
