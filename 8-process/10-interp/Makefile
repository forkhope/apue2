CC = gcc
CFLAGS = -Wall -o
BINS = echoarg interp

.PHONY: all do

%: %.c
	$(CC) $(CFLAGS) $@ $^

all: $(BINS) do

echoarg: echoarg.c
interp: interp.c

do:
	echo -e '\033[32mEXECUTE THE BIN !!!\033[0m'
	./interp
