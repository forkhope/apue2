.PHONY: all do

%: %.c
	gcc -Wall -o $@ $<

all: line do

line: line.c

do:
	echo -e '\033[32mEXECUTE THE BIN!!!\033[0m'
	echo -n "tianxiayouqingren" | ./line
