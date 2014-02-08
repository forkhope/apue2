CC = gcc
CFLAGS = -Wall
LFLAGS = -Wall -o
OBJS = $(patsubst %.c,%.o,$(wildcard *.c))
MYUCLC=myuclc
BIN = filter

.PHONY: all clean do

all: $(MYUCLC) $(BIN) clean do

$(MYUCLC): myuclc.o
	$(CC) $(LFLAGS) $@ $^

$(BIN): filter.o
	$(CC) $(LFLAGS) $@ $^

clean:
	rm -f $(OBJS)

do:
	echo -e '\033[31mRUN THE PROGREAM!!!\033[0m'
	./filter
