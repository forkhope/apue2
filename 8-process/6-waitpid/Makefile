CC = gcc
CFLAGS = -Wall -c -o
LFLAGS = -Wall -o
OBJ = waitpid.o
BIN = waitpid

.PHONY: all clean do

all: $(OBJ) $(BIN) clean do
%.o: %.c
	$(CC) $(CFLAGS) $@ $<

waitpid.o: waitpid.c

$(BIN): $(OBJ)
	$(CC) $(LFLAGS) $@ $^

clean:
	rm -f $(OBJ)

do:
	echo -e "\033[32mEXECUTE THE BIN\033[0m"
	./waitpid
