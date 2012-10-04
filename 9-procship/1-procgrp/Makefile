.PHONY: all do

%: %.c
	gcc -Wall -D_XOPEN_SOURCE=500 -o $@ $<

all: procgrp do

procgrp: procgrp.c

do:
	echo -e '\033[31mEXECUTE THE BIN!!!\033[0m'
	./procgrp
