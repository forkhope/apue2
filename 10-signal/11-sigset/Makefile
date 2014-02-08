.PHONY: all do

all: sigset do

%: %.c
	gcc -Wall -o $@ $<

do:
	echo -e '\033[31mEXECUTE THE BIN!!!\33[0m'
	./sigset
