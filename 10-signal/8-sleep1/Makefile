CC = gcc
CFLAGS = -Wall -c -o
LFLAGS = -Wall -o
OBJS = sleep1.o usesleep1.o
BIN = usesleep1

.PHONY: all do clean

%.o: %.c
	$(CC) $(CFLAGS) $@ $<

%: %.o
	$(CC) $(LFLAGS) $@ $^

all: $(BIN) clean do

$(BIN): $(OBJS)

do:
	echo -e '\033[31mEXECUTE THE BIN!!!\033[0m'
	./usesleep1

clean: 
	rm -f $(OBJS)
