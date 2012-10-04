.PHONY: all do

%: %.c
	gcc -Wall -o $@ $<

all: st_dev do


st_dev: st_dev.c

do:
	echo -e '\033[32mEXECUTE THE BIN!!!\033[0m'
	./st_dev / /home/lixianyi /media/windows1 /etc /dev/tty[01]
