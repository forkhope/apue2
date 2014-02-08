.PHONY: all do

%: %.c
	gcc -Wall -o $@ $<

all: reentrant do

reentrant: reentrant.c

do:
	echo -e '\033[31mEXECUTE THE BIN!!!\033[0m'
	./reentrant
