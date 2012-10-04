all: vfork do

%: %.c
	gcc -Wall -o $@ $<

vfork: vfork.c

do:
	echo -e '\033[32mEXECUTE THE BIN!!!\033[0m'
	./vfork
