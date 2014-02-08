CC = gcc
CFLAGS = -Wall -o
SRC = onlret.c
BIN = onlret

%: %.c
	$(CC) $(CFLAGS) $@ $<

all: $(BIN) do

$(BIN): $(SRC)

do:
	echo -e '\033[32mRUN THE PROGRAM~~~~\033[0m'
	./onlret
