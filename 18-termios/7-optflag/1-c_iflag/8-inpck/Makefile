CC = gcc
CFLAGS = -Wall -o
SRC = inpck.c
BIN = inpck

%: %.c
	$(CC) $(CFLAGS) $@ $<

all: $(BIN) do

$(BIN): $(SRC)

do:
	echo -e '\033[32mRUN THE PROGRAM~~~~\033[0m'
	./inpck
