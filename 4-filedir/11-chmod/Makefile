.PHONY: all do

%: %.c
	gcc -Wall -o $@ $<

all: chmod do

chmod: chmod.c

do:
	echo -e '\033[32mEXECUTE THE BIN !!! \033[0m'
	rm -f foo bar
	touch foo bar
	ls -l foo bar
	./chmod
	ls -l foo bar
