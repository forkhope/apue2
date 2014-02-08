CC = gcc
CFLAGS = -Wall -o
SRC = cmspar.c
BIN = cmspar

%: %.c
	$(CC) $(CFLAGS) $@ $<

all: $(BIN) do

$(BIN): $(SRC)

do:
	echo -e '\033[32mRUN THE PROGRAM~~~~\033[0m'
	./cmspar
