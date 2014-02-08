CC = gcc
CFLAGS = -Wall -o
SRC = disable.c
BIN = disable

%: %.c
	$(CC) $(CFLAGS) $@ $<

all: $(BIN) do

$(BIN): $(SRC)

do:
	echo -e '\033[32mRUN THE PROGRAM~~~~\033[0m'
	stty -a
	./disable
	stty -a
