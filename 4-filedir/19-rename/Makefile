.PHONY: all do

%: %.c
	gcc -Wall -o $@ $<

all: rename do

rename: rename.c

do:
	touch tian
	echo -e '\033[32mEXECUTE THE BIN !!! \033[0m'
	./rename
