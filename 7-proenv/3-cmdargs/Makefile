.PHONY: all do

%: %.c
	gcc -Wall -o $@ $<

all: cmdargs do

cmdargs: cmdargs.c

do:
	echo -e '\033[32mEXECUTE THE BIN!!!\033[0m'
	./cmdargs arg1 arg2foo arg3bar
