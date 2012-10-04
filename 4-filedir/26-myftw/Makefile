CC = gcc
CFLAGS = -Wall -Iinclude -c -o
LFLAGS = -Wall -o
OBJS = pathname.o errhandle.o myftw.o
BIN = myftw

.PHONY: all clean

all: $(BIN) clean

$(BIN): $(OBJS)
	$(CC) $(LFLAGS) $@ $^

clean:
	rm -f $(OBJS)
