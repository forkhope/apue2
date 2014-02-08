CC = gcc
CFLAGS = -Wall -o
SRC = imaxbel.c
BIN = imaxbel

%: %.c
	$(CC) $(CFLAGS) $@ $<

all: $(BIN) do

$(BIN): $(SRC)

do:
	echo -e '\033[32mRUN THE PROGRAM~~~~\033[0m'
	./imaxbel
