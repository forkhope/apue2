CC = gcc
CFLAGS = -Wall -Iinclude
LFLAGS = -Wall -o
OBJS = errhandle.o copyfile.o
BIN = copyfile

.PHONY: all clean

all: $(BIN) clean

$(BIN): $(OBJS)
	$(CC) $(LFLAGS) $@ $^

clean:
	rm -f $(OBJS)
