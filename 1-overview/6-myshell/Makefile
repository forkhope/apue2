CC = gcc
CFLAGS = -Wall -Iinclude -c -o
LFLAGS = -Wall -o
OBJS = errhandle.o myshell.o
BIN = myshell

.PHONY: all clean

all: $(BIN) clean

$(BIN): $(OBJS)
	$(CC) $(LFLAGS) $@ $^

clean:
	rm -f $(OBJS)
