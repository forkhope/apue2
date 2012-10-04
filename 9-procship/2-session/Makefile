.PHONY: all do

%: %.c
	gcc -Wall -o $@ $<

all: session do 

session: session.c

do:
	echo -e '\033[31mEXECUTE THE BIN!!!\033[0m'
	./session
