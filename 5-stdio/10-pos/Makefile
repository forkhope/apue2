.PHONY: all do

%: %.c
	gcc -Wall -o $@ $<

all: position do

position: position.c

do:
	echo -e '\033[32mEXECUTE THE BIN!!!\033[0m'
	./position
