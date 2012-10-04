.PHONY: all do

%: %.c
	gcc -Wall -o $@ $<

all: printf do

printf: printf.c

do:
	echo -e '\033[32mEXECUTE THE BIN!!!\033[0m'
	./printf
