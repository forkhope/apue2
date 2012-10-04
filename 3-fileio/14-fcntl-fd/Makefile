CC = gcc
CFLAGS = -Wall -Iinclude -c -o
LFLAGS = -Wall -o
OBJS = errhandle.o fcntl-fd.o
BIN = fcntl-fd

.PHONY: all clean do

all: $(BIN) clean do

$(BIN): $(OBJS)
	$(CC) $(LFLAGS) $@ $^

clean:
	rm -f $(OBJS)

do:
	echo -e "\033[32mEXECUTE THE BIN !!!\033[0m"
	./fcntl-fd
