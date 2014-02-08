CC = gcc
CFLAGS = -Wall
LFLAGS = -Wall -o
OBJS = $(patsubst %.c,%.o,$(wildcard *.c))
BIN = sigsetjmp

.PHONY: all do clean

all: $(BIN) clean do

$(BIN): $(OBJS)
	$(CC) $(LFLAGS) $@ $^

do:
	echo -e '\033[31mEXECUTE THE BIN!!!\033[0m'
	./sigsetjmp

clean:
	rm -f $(OBJS)
