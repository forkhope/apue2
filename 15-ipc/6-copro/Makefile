CC = gcc
LFLAGS = -Wall -o

all: add2 copro do

add2: add2.c
	$(CC) $(LFLAGS) $@ $^

copro: copro.c
	$(CC) $(LFLAGS) $@ $^

do:
	echo -e '\033[31mRUN THE PROGRAM!!!\033[0m'
	./copro
