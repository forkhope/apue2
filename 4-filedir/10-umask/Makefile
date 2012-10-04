.PHONY: all do

%: %.c
	gcc -Wall -o $@ $^

all: umask do

umask: umask.c

do:
	echo -e '\033[32mEXECUTE THE BIN !!!\033[0m'
	./umask
	ls -l foo bar
	rm -f foo bar
