CC = gcc
CFLAGS = -Wall -Iinclude -c -o
LFLAGS = -Wall -o
OBJS = errhandle.o fcntl-fl.o
BIN = fcntl-fl

.PHONY: all clean

all: $(BIN) clean

$(BIN): $(OBJS)
	$(CC) $(LFLAGS) $@ $^
	
clean:
	rm -f $(OBJS)
