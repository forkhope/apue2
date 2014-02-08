CC = gcc
CFLAGS = -Wall -c -o
LFLAGS = -Wall -o
OBJS = sleep2.o usesleep2.o
BIN = usesleep2

.PHONY: all clean do

%.o: %.c
	$(CC) $(CFLAGS) $@ $<

%: %.o
	$(CC) $(LFLAGS) $@ $^

all: $(BIN) clean do

$(BIN): $(OBJS)

clean:
	rm -f $(OBJS)

do:
	echo -e '\033[31mEXECUTE THE BIN!!!\033[0m'
	./usesleep2
