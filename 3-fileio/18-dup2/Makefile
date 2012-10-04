CC = gcc
CFLAGS = -Wall -Iinclude -c -o
LFLAGS = -Wall -o
OBJS = errhandle.o use-dup2.o dup2.o
BIN = use-dup2

.PHONY: all clean do

all: $(BIN) clean do

$(BIN): $(OBJS)
	$(CC) $(LFLAGS) $@ $^

clean:
	rm -f $(OBJS)

do:
	echo -e "\033[32mEXECUTE THE BIN !!!\033[0m"
	./use-dup2
