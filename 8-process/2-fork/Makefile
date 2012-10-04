.PHONY: all do

%: %.c
	gcc -Wall -o $@ $<

all: fork do

fork: fork.c

do:
	echo -e '\033[32mEXECUTE THE BIN!!!\033[0m'
	./fork
	@echo ===================================
	./fork > tempout
	cat tempout
