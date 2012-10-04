.PHONY: all do

all: longjmp longjmp-o do

longjmp: longjmp.c
	gcc -Wall -o $@ $<

longjmp-o: longjmp.c
	gcc -Wall -O -o $@ $<

# 下面在 echo 前面加 @,表示这条命令不回显,即这条命令本身不会被打印
do:
	echo -e '\033[32mEXECUTE THE BIN!!!\033[0m'
	./longjmp
	@echo ====================================
	./longjmp-o
