CC = gcc
CFLAGS = -Wall -o
SRCS = $(shell echo *.c)
BINS = $(SRCS:.c=)

.PHONY: all do

all: $(BINS) do

%: %.c
	$(CC) $(CFLAGS) $@ $<

do:
	echo -e '\033[31mEXECUTE THE BIN!!!\033[0m'
	./sigpend
