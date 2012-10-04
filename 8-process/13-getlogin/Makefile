%: %.c
	gcc -Wall -o $@ $<

.PHONY: all do

all: getlogin do

getlogin: getlogin.c

do:
	echo -e '\033[31mEXECUTE THE BIN!!!\033[0m'
	./getlogin
