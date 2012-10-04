.PHONY: all do

%: %.c
	gcc -Wall -o $@ $<

all: unlink do

unlink: unlink.c

do:
	echo -e '\033[32mEXECUTE THE BIN !!! \033[0m'
	touch tian
	ln tian hlink-tian
	ln -s tian slink-tian
	./unlink
