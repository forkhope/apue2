CC = gcc
CFLAGS = -Wall
LFLAGS = -Wall -o
OBJ = $(patsubst %.c,%.o,$(wildcard *.c))
BIN = status

.PHONY: all clean do

all: $(BIN) clean do

$(BIN): $(OBJ)
	$(CC) $(LFLAGS) $@ $^

clean:
	rm -f $(OBJ)

do:
	echo -e '\033[31mRUN THE PROGRAM!!!!\033[0m'
	./status
