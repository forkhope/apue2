CC = gcc
CFLAGS = -Wall -o
SRC = isatty.c
BIN = isatty

%: %.c
	$(CC) $(CFLAGS) $@ $<

all: $(BIN) do

$(BIN): $(SRC)

do:
	echo -e '\033[32mRUN THE PROGRAM~~~~\033[0m'
	./isatty
	./isatty <Makefile 2>/dev/null
