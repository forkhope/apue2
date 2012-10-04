.PHONY: all do

%: %.c
	gcc -Wall -std=c99 -o $@ $<

all: fwide do

fwide: fwide.c

do:
	echo -e '\033[32mEXECUTE THE BIN!!!\033[0m'
	touch tian
	./fwide
