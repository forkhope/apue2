CC = gcc
# 注意,下面的 CFLAGS 不能写为 -Wall -o, 因为在这个Makefile中,没有显示的编译
# .c 文件,而是是用了makefile隐含规则来将.c文件编译为.o文件,后面的sigaction
# 目标直接使用了.o文件来编译为可执行文件.当使用makefile隐含规则来将.c文件编
# 译为.o文件时,它会用到CFLAGS变量,且会自动添加 -c -o 选项到 CFLAGS 变量中.
# 例如下面定义 CFLAGS 为 -Wall -c -o,编译时,make命令打印出来的实际命令是:
# 		gcc -Wall -c -o   -c -o sigaction.o sigaction.c
# 可以看到有两个 -c -o,其中一个是文件中定义的,另一个是makefile的隐含规则自动
# 添加进去的. 如果将 CFLAGS 定义为 -Wall -o,则编译时的实际命令是:
# 		gcc -Wall -o -c -o sigaction.o sigaction.c
# 这样编译出来的 sigaction.o 实际上是 ELF 可执行格式,而不是ELF重定向格式,
# 这样在后面的 sigaction 目标中,执行 gcc -Wall -o sigaction sigaction.o 时
# 会报错. 我本来是只定义了 CFLAGS = -Wall -o,但是这样写有问题,才把 CFLAGS
# 定义为 -Wall -c -o,又另外定义了 LFLAGS 变量
CFLAGS = -Wall -c -o
LFLAGS = -Wall -o

# GNU make手册的4.4.3小节提到: one use of the wildcard function is to get a
# list of all the C source files in a directory, like this: $(wildcard *.c).
# We can change the list of C source files into a list of object files by
# replacing the '.c' suffix with '.o' in the result, like this:
# $(patsubst %.c,%.o,%(wildcard *.c))
# patsubst函数的格式是: $(patsubst pattern,replacement,text),详见 8.2 小节
OBJS := $(patsubst %.c,%.o,$(wildcard *.c))

all: sigaction clean do 

sigaction: $(OBJS)
	$(CC) $(LFLAGS) sigaction $(OBJS)

do:
	echo -e '\033[31mEXECUTE THE BIN!!!\033[0m'
	./sigaction

clean:
	rm -f $(OBJS)
