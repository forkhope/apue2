CC = gcc
CFLAGS = -Wall -Iinclude
LFLAGS = -Wall -pthread -o
OBJS = $(patsubst %.c,%.o,$(wildcard *.c))
BIN = errexit

.PHONY: all clean do

all: $(BIN) clean do

$(BIN): $(OBJS)
	$(CC) $(LFLAGS) $@ $^

clean:
	rm -f $(OBJS)

do:
	echo -e '\033[31mEXECUTE THE BIN!!!\033[0m'
	./errexit
