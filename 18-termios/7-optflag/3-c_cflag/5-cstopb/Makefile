CC = gcc
CFLAGS = -Wall -o
SRC = cstopb.c
BIN = cstopb

%: %.c
	$(CC) $(CFLAGS) $@ $<

all: $(BIN) do

$(BIN): $(SRC)

do:
	echo -e '\033[32mRUN THE PROGRAM~~~~\033[0m'
	./cstopb
