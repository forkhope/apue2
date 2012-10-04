.PHONY: all do

%: %.c
	gcc -Wall -o $@ $<

all: environ do

environ: environ.c

do:
	echo -e '\033[32mEXECUTE THE BIN!!!\033[0m'
	./environ
