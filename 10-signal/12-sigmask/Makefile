CC = gcc
CFLAGS = -Wall -o

# Makefile 中可以使用函数来处理变量.函数的调用,和变量的使用类似,也是用 '$'
# 来标识的,其语法为: $(<function> <args>),或者是 ${<function>, <args>}
# 这里,<function>就是函数名,<args>是函数的参数,多个参数间用逗号','分割.而
# 函数名和参数名之间用"空格"分割.
# 下面就是调用了 Makefile 中的 shell 函数,它的参数是shell的命令.该函数返回
# 所指定shell命令的结果.
SRCS = $(shell echo *.c)

# GNU make手册的 6.3.1 小节提到一个"Substitution References"的特性,如下:
# A substitution reference substitutes the value of a variable with
# alterations that you specify: It has the form '$(var:a=b)' (or 
# '${var:a=b}') and its meaning is to take the value of the variable "var",
# replace every "a" at the end of a word with "b" in that value, and 
# substitute the resulting string. 这个特性只能替换词中的末尾字符串.
# When we say "at the end of a word", we mean that "a" must appear either
# followed by whitespace or at the end of the value in order to be replaced;
# other occurrences of "a" in the value are unaltered.
# 注意, "var:" 和 "a" 之间最好不要加空格,否则可能会出错.
# 下面语句去掉 SRCS 变量中以 .c 结尾的词中的 .c
BINS = $(SRCS:.c=)

.PHONY: all do

all: $(BINS) do

%: %.c
	$(CC) $(CFLAGS) $@ $<

do:
	echo -e '\033[31mEXECUTE THE BIN!!!\033[0m'
	./sigmask
