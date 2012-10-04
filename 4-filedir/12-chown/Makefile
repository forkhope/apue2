.PHONY: all do
%: %.c
	gcc -Wall -o $@ $^

all: chown do

chown: chown.c

do:
	echo -e '\033[32mEXECUTE THE BIN !!! \033[0m'
	./chown
