CC = gcc
CFLAGS = -Wall -Iinclude -c -o
LFLAGS = -Wall -o
OBJS = errhandle.o filetype.o
BIN = filetype

.PHONY: all clean do

all: $(BIN) clean do

$(BIN): $(OBJS)
	$(CC) $(LFLAGS) $@ $^

clean:
	rm -f $(OBJS)

do:
	echo -e '\033[32mEXECUTE THE BIN !!! \033[0m'
	./filetype /etc/passwd /etc /dev/initctl /dev/log /dev/tty \
		/dev/loop0 /dev/cdrom
