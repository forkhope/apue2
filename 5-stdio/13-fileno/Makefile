.PHONY: all do

%: %.c
	gcc -Wall -o $@ $<

all: fileno do

fileno: fileno.c

do:
	echo -e '\033[32mEXECUTE THE BIN!!!\033[0m'
	./fileno
