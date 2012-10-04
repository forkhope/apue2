.PHONY: all do

all: write do

write: write.c
	gcc -Wall -o $@ $<

do:
	echo -e "\033[32mEXECUTE THE BIN !!! \033[0m"
	./write
