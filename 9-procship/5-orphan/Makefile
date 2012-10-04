.PHONY: all do

%: %.c
	gcc -Wall -o $@ $<

all: orphan do

orphan: orphan.c

do:
	echo -e '\033[31mEXECUTE THE BIN!!!\033[0m'
	./orphan
