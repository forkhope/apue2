.PHONY: all do

%: %.c
	gcc -Wall -o $@ $<

all: proctime do

proctime: proctime.c

do:
	echo -e '\033[31mEXECUTE THE BIN!!!\033[0m'
	./proctime "sleep 5" "date"
