CC = gcc
CFLAGS = -Wall -Iinclude
LFLAGS = -Wall -o
OBJS = errhandle.o filesize.o
BIN = filesize

.PHONY: all clean

all: $(BIN) clean

$(BIN): $(OBJS)
	$(CC) $(LFLAGS) $@ $^

clean:
	rm -f $(OBJS)
