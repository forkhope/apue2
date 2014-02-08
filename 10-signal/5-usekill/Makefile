.PHONY: all do

%: %.c
	gcc -Wall -o $@ $<

all: usekill do

usekill: usekill.c

do:
	echo -e '\033[31mEXECUTE THE BIN!!!\033[0m'
	./usekill
