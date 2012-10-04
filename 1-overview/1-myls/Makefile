CC = gcc
CFLAGS = -Wall -Iinclude
LFLAGS = -Wall -o
OBJS = myls.o errhandle.o
BIN = myls

.PHONY: all clean

all: $(BIN) clean

$(BIN): $(OBJS)
	$(CC) $(LFLAGS) $@ $^

clean:
	rm -f $(OBJS)
