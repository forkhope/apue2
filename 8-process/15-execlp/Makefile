CC = gcc
CFLAGS = -Wall -o
BINS = echoarg execlp

.PHONY: all do

%: %.c
	$(CC) $(CFLAGS) $@ $^

all: $(BINS) do

echoarg: echoarg.c
interp: execlp

do:
	echo -e '\033[32mEXECUTE THE BIN !!!\033[0m'
