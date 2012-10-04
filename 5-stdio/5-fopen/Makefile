.PHONY: all do

%: %.c
	gcc -Wall -o $@ $<

all: fopen do

fopen: fopen.c

do:
	echo -e '\033[32mEXECUTE THE BIN!!!\033[0m'
	rm -f tian
	./fopen
