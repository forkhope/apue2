CC = gcc
CFLAGS = -Wall -Iinclude
LFLAGS = -Wall -o
OBJS = errhandle.o signals.o
BIN = signals

.PHONY: all clean

all: $(BIN) clean

$(BIN): $(OBJS)
	$(CC) $(LFLAGS) $@ $^

clean:
	rm -f $(OBJS)
