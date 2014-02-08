all: alarmread do

alarmread: alarmread.c
	gcc -Wall -o $@ $<

do:
	echo -e '\033[31mEXECUTE THE BIN!!!\033[0m'
	./alarmread
