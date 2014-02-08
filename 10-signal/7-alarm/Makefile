.PHONY: all do

%: %.c
	gcc -Wall -o $@ $<

all: alarm do

alarm: alarm.c

do:
	echo -e '\033[31mEXECUTE THE BIN!!!\033[0m'
	./alarm
