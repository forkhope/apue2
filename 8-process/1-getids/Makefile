.PHONY: all do

%: %.c
	gcc -Wall -o $@ $<

all: getids do

getids: getids.c

do:
	echo -e '\033[32mEXECUTE THE BIN!!!\033[0m'
	./getids
