CC = gcc
CFLAGS = -Wall -c -o
LFLAGS = -Wall -o
OBJS = $(patsubst %.c,%.o,$(wildcard *.c))
BIN = waitsig

.PHONY: all do clean

all: $(BIN) clean do

$(BIN): $(OBJS)
	$(CC) $(LFLAGS) $@ $^

clean:
	rm -f $(OBJS)

do:
	echo -e '\033[31mEXECUTE THE BIN!!!\033[0m'
	./waitsig
