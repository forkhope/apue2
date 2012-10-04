.PHONY: all do

%: %.c
	gcc -Wall -o $@ $<

all: getenv do

getenv: getenv.c

do:
	echo -e '\033[32mEXECUTE THE BIN!!!\033[0m'
	./getenv
