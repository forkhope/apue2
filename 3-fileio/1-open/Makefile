CC = gcc
CFLAGS = -Wall -Iinclude
LFLAGS = -Wall -o
OBJS = errhandle.o open.o
BIN = open

.PHONY: all clean do

all: $(BIN) clean do

$(BIN): $(OBJS)
	$(CC) $(LFLAGS) $@ $^

clean:
	rm -f $(OBJS) *.txt

do:
	echo -e '\033[32mEXECUTE THE BIN !!! \033[0m'
	./open
