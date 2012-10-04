.PHONY: all do

%: %.c
	gcc -Wall -o $@ $<

all: cngenv do

cngenv: cngenv.c

do:
	echo -e '\033[32mEXECUTE THE BIN!!!\033[0m'
	./cngenv
