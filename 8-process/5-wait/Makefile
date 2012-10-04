CC = gcc
CFLAGS = -Wall -c -o
LFLAGS = -Wall -o
OBJ = pr_exit.o wait.o
BIN = wait

.PHONY: all clean do

all: $(OBJ) $(BIN) clean do
%.o: %.c
	$(CC) $(CFLAGS) $@ $<

pr_exit.o: pr_exit.c
wait.o: wait.c

$(BIN): $(OBJ)
	$(CC) $(LFLAGS) $@ $^

clean:
	rm -f $(OBJ)

do:
	echo -e "\033[32mEXECUTE THE BIN\033[0m"
	./wait
