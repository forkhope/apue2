.PHONY: all do

%: %.c
	gcc -Wall -o $@ $<

all: socketpair do

socketpair: socketpair.c

do:
	echo -e '\033[31mEXECUTE THE BIN!!!\033[0m'
	./socketpair
