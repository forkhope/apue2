CC = gcc
CFLAGS = -Wall -o
SRC = setids.c
BIN = setids

%: %.c
	$(CC) $(CFLAGS) $@ $^

.PHONY: all do

all: $(BIN) do

$(BIN): $(SRC)

do:
	echo -e '\033[32mEXECUTE THE BIN !!! \033[0m'
	./setids /etc/passwd
	./setids /usr/bin/passwd
