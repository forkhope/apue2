.PHONY: all do

%: %.c
	gcc -Wall -o $@ $<

all: binio do

binio: binio.c

do:
	echo -e '\033[32mEXECUTE THE BIN!!!\033[0m'
	echo -n "1234567891234567" | ./binio
