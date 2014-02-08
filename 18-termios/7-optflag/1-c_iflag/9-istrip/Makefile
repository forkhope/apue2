CC = gcc
CFLAGS = -Wall -o
SRC = istrip.c
BIN = istrip

%: %.c
	$(CC) $(CFLAGS) $@ $<

all: $(BIN) do

$(BIN): $(SRC)

do:
	echo -e '\033[32mRUN THE PROGRAM~~~~\033[0m'
	./istrip
