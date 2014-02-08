CC = gcc
CFLAGS = -Wall
LFAGS = -Wall -o
OBJ = $(patsubst %.c,%.o,$(wildcard *.c))
BIN = canonical

.PHONY: all clean do

all: $(BIN) clean do

$(BIN): $(OBJ)
	$(CC) $(LFAGS) $@ $^

clean:
	rm -f $(OBJ)

do:
	echo -e '\033[31mRUN THE PROGRAM!!!\033[0m'
	./canonical
