CC = gcc
CFLAGS = -Wall -D_XOPEN_SOURCE=601 -o
SRC = openpt.c
BIN = openpt

%: %.c
	$(CC) $(CFLAGS) $@ $<

all: $(BIN) do

$(BIN): $(SRC)

do:
	echo -e '\033[32mRUN THE PROGRAM~~~~\033[0m'
	./openpt
