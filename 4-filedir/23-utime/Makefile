.PHONY: all do

%: %.c
	gcc -Wall -o $@ $<

all: utime do

utime: utime.c

do:
	echo -e '\033[32mEXECUTE THE BIN !!!\033[0m'
	./utime
