.PHONY: all do

%: %.c
	gcc -Wall -o $@ $<

all: rmdir do

rmdir: rmdir.c

do:
	echo -e '\033[32mEXECUTE THE BIN!!!\033[0m'
	mkdir test
	./rmdir
