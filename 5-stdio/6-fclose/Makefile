.PHONY: all do

%: %.c
	gcc -Wall -o $@ $<

all: fclose do

fclose: fclose.c

do:
	echo -e '\033[32mEXECUTE THE BIN!!!\033[0m'
	./fclose
