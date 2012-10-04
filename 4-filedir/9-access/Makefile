.PHONY: all do

%: %.c
	gcc -Wall -o $@ $^

all: access do

access: access.c

do:
	echo -e '\033[32mEXECUTE THE BIN !!! \033[0m'
	./access access.c
	./access access
	./access /etc/shadow
	./access tian
