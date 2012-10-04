.PHONY: all do

%: %.c
	gcc -Wall -o $@ $<

all: atexit do

atexit: atexit.c

do:
	echo -e '\033[32mEXECUTE THE BIN!!!\033[0m'
	./atexit
