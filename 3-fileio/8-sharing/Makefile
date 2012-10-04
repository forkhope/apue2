.PHONY: all do

all: a b do

a: a.c
	gcc -Wall -o $@ $<

b: b.c
	gcc -Wall -o $@ $<

do:
	echo -e "\033[32mEXECUTE THE BIN !!!\033[0m"
	rm -f yu ui
	./a &
	./b &
