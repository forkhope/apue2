.PHONY: all do

%: %.c
	gcc -Wall -o $@ $<

all: remove do

remove: remove.c

do:
	touch tian
	echo -e '\033[32mEXECUTE THE BIN !!!\033[0m'
	./remove
