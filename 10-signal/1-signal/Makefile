.PHONY: all do

%: %.c
	gcc -Wall -o $@ $<

all: signal do

signal: signal.c

do:
	echo -e '\033[31mEXECUTE THE BIN!!!\033[0m'
	./signal &
	ps
