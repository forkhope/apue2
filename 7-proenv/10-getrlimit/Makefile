.PHONY: all do

%: %.c
	gcc -Wall -o $@ $<

all: getrlimit do

getrlimit: getrlimit.c

do:
	echo -e '\033[32mEXECUTE THE BIN!!!\033[0m'
	./getrlimit
