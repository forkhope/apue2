.PHONY: all do

%: %.c
	gcc -Wall -o $@ $<

all: mkdir do

mkdir: mkdir.c

do:
	echo -e '\033[32mEXECUTE THE BIN!!!\033[0m'
	rm -rf test
	./mkdir test
