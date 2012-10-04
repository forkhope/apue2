.PHONY: all do

%: %.c
	gcc -Wall -o $@ $<

all: fflush do

fflush: fflush.c

do:
	echo -e '\033[32mEXECUTE THE BIN!!!\033[0m'
	./fflush
