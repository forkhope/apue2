.PHONY: all do

%: %.c
	gcc -Wall -o $@ $<

all: setjmp do

setjmp: setjmp.c

do:
	echo -e '\033[32mEXECUTE THE BIN!!!\033[0m'
	./setjmp
