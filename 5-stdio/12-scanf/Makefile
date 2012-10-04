.PHONY: all do

%: %.c
	gcc -Wall -o $@ $<

all: scanf do

scanf: scanf.c

do:
	echo -e '\033[32mEXECUTE THE BIN!!!\033[0m'
	./scanf
