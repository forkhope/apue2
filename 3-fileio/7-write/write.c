#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>

/* Data is written to an open file with the write() function.
 * #include <unistd.h>
 * ssize_t write(int filedes, const void *buf, size_t nbytes);
 * Return: number of bytes written if OK, -1 on error.
 *
 * The return value is usually equal to the nbytes argument; otherwise, an
 * error has occurred. For a regular file, the write starts at the file's
 * current offset. If the O_APPEND option was specified when the file was
 * opened, the file's offset is set to the current end of file.
 *
 * 程序中提到,字符串是以'\0'结尾,文件是以EOF结尾; '\0'的整数值是0, EOF的
 * 整数值是-1,两者之间是不同的. 标准I/O函数除了 fwrite() 和 fread() 之外,
 * 其他大部分函数都以字符串为操作对象,遇到'\0'会结束操作.部分带有FILE *
 * 参数的读取函数,如fgets(),getchar()等遇到EOF也会结束操作,因为它们已经读
 * 完了文件内容,无法再读了,所以会终止. 而linux系统函数write()和read()以文
 * 件为操作对象,遇到EOF或者最大字节数时才会结束操作,遇到'\0'并不会结束操作.
 *
 * 当然,要明确的是,文件的末尾并不真实存在一个EOF,仅仅是说,当读到文件末尾时,
 * read() 函数会返回一个 -1,表示已经读到了文件末尾.程序中用EOF宏来表示 -1.
 */
int main(void)
{
	size_t n;
	int fd;
	char buf[24];

	if ((fd = open("a.txt", O_RDWR | O_CREAT | O_TRUNC, 0644)) < 0) {
		printf("write: can't open a new file (a.txt): %m\n");
		return 1;
	}

	n = strlen("tianxia");
	if (write(fd, "tianxia", n) != n) {
		printf("write: can't write to a.txt: %s\n", strerror(errno));
		return 1;
	}
	/* After a successful write, the file's offset is incremented by the
	 * number of bytes actually written. 因此,下面的read()函数实际上是从
	 * 刚才写入的字符串后面读起,由于后面并没有内容,会读到EOF,即什么也读不
	 * 到,没有任何有效字符写入到 buf 中,打印出来的 buf 为空.
	 */
	if (read(fd, buf, 24) < 0) {
		printf("write: can't read from a.txt: %s\n", strerror(errno));
		return 1;
	}
	/* 因为 buf 没有初始化,里面的内容是随机的,打印出来的内容一般是乱码 */
	printf("read buf (这个读到的将全是乱码): %s\n", buf);

	/* 为了避免上面提到的问题,将文件偏移指针移到文件开始处 */
	if (lseek(fd, 0, SEEK_SET) < 0) {
		printf("write: can't lseek to the beginning of a.txt: %m\n");
		return 1;
	}
	if (read(fd, buf, 24) < 0) {
		printf("write: can't read from a.txt: %s\n", strerror(errno));
		return 1;
	}
	/* 由于上面的 write() 函数只写入了 7 个字符,没有写入第8个字符'\0',则
	 * 此时文件内容没有包含'\0',导致下面读出的字符串并没有默认就以'\0'结
	 * 尾,打印出来的 buf 的后面部分将是乱码.
	 */
	printf("read buf (后面部分将是乱码): %s\n", buf);

	printf("After read(), offset = %ld\n", lseek(fd, 0, SEEK_CUR));

	/* 由于 read() 函数执行结束后,文件偏移指针已经指向文件末尾,不需要
	 * 再次调用 lseek() 函数,直接调用 write() 将在文件末尾写入新的数据.
	 */
	if (write(fd, "-tianxi", n+1) != n+1) {
		printf("write: can't write to a.txt: %s\n", strerror(errno));
		return 1;
	}
	if (lseek(fd, 0, SEEK_SET) < 0) {
		printf("write: can't lseek to the beginning of a.txt: %m\n");
		return 1;
	}
	/* 上面的 write() 函数写入的是 8 个字符,包括第8个字符'\0',则此时文件
	 * 的末尾字符就是'\0',下面读出的字符串默认就以'\0'结尾,打印的buf将不
	 * 会有乱码.这里说明, write()函数不会对'\0'字符作特别的处理,也会写入.
	 */
	if (read(fd, buf, 24) < 0) {
		printf("write: can't read from a.txt: %s\n", strerror(errno));
		return 1;
	}
	printf("read buf (由于写入'\\0',这里没有乱码): %s\n", buf);



	/* 由于此时文件末尾是'\0'字符,而 read() 函数也不会对 '\0' 字符作特别
	 * 处理,一样会读出该字符,则如果使用标准I/O函数来操作读出的字符串,'\0'
	 * 后面的内容会无法读出.因为标准I/O函数遇到'\0'就终止了,不再往下处理.
	 */
	if (write(fd, "tia\n", 4) != 4) {
		printf("write: can't write to a.txt: %s\n", strerror(errno));
		return 1;
	}
	if (lseek(fd, 0, SEEK_SET) < 0) {
		printf("write: can't lseek to the beginning of a.txt: %m\n");
		return 1;
	}
	/* 上面的 write() 函数执行完后, a.txt 包含有19个字节,包含前面写入的
	 * 14 个字节和 1 个'\0'字符,和刚才写入的 "tia\n" 这 4 个字节,但是由
	 * 于 printf() 函数遇到'\0'就终止执行,后面的"tian\n"将无法通过printf()
	 * 函数来打印出来.但是通过write()函数可以全部打印,如下面的代码所示.
	 */
	if ((n = read(fd, buf, 24)) < 0) {
		printf("write: can't read from a.txt: %s\n", strerror(errno));
		return 1;
	}
	printf("read buf ('\\0'后面的tia\\n没有被printf打出): %s\n", buf);
	printf("read buf (调用fwrite()函数来打印,将打印'\\0'后面的tia\\n:");
	fwrite(buf, 19, 1, stdout);	 // 在字符中间有个'\0',所以是19个字符
	printf("read buf (调用write()函数来打印,将打印'\\0'后面的tia\\n:");
	fflush(NULL);	// 如果没有这一句,后面的write()的内容会被先打出.
	write(STDOUT_FILENO, buf, n); // 文件后面的"tia\n"将被打印出来

	close(fd);
	return 0;
}
