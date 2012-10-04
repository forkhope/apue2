.PHONY: all do

%: %.c
	gcc -Wall -o $@ $<

all: exec echoall do

exec: exec.c

echoall: echoall.c

do:
	echo -e '\033[32mEXECUTE THE BIN!!!\033[0m'
	./exec
