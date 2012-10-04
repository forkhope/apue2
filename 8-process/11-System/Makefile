CC = gcc
CFLAGS = -Wall -c -o
LFLAGS = -Wall -o
OBJS = System.o pr_exit.o useSystem.o
BIN = useSystem

.PHONY: all clean do

%.o: %.c
	$(CC) $(CFLAGS) $@ $<

%: %.o
	$(CC) $(LFLAGS) $@ $^

all: $(OBJS) $(BIN) clean do

System.o: System.c
pr_exit.o: pr_exit.c
useSystem.o: useSystem.c

useSystem: useSystem.o pr_exit.o System.o

clean:
	rm -f $(OBJS)

do:
	echo -e '\033[31mEXECUTE THE BIN !!!\033[0m'
	./useSystem
