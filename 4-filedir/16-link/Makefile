.PHONY: all do

%: %.c
	gcc -Wall -o $@ $^

all: link do

link: link.c

do:
	echo -e '\033[32mEXECUTE THE BIN !!! \033[0m'
	rm -f hlink-tian
	./link
