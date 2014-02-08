all: detach do

detach: detach.c
	gcc -Wall -pthread -o $@ $<

do:
	echo -e '\033[31mRUN THE PROGRAM!!!\033[0m'
	./detach
