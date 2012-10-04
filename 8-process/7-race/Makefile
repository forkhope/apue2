.PHONY: all do

%: %.c
	gcc -Wall -o $@ $<

all: race do

race: race.c

do:
	echo -e '\033[32mEXECUTE THE BIN\033[0m'
	./race
