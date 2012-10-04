.PHONY: all do

%: %.c
	gcc -Wall -o $@ $<

all: zombie do

zombie: zombie.c

do:
	echo -e '\033[31mEXECUTE THE BIN!!!\033[0m'
	./zombie
