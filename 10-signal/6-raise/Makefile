.PHONY: all do

%: %.c
	gcc -Wall -o $@ $<

all: raise do

raise: raise.c

do:
	echo -e '\033[31mEXECUTE THE BIN!!!\033[0m'
	./raise
