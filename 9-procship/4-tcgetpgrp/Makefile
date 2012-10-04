.PHONY: all do

%: %.c
	gcc -Wall -o $@ $<

all: tcgetpgrp do

tcgetpgrp: tcgetpgrp.c

do:
	echo -e '\033[31mEXECUTE THE BIN!!!\033[0m'
	./tcgetpgrp
