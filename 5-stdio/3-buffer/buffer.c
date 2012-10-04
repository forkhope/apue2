#include <stdio.h>
#include <string.h>
#include <errno.h>

#define BUFSIZZ  (12)

/* The standard I/O library tries to do its buffering automatically for
 * each I/O stream, obviating the need for the application to worry about it
 * In the standard I/O library, three types of buffering are provided:
 * 1. Fully buffered. In this case, actual I/O takes place when the standard
 *    I/O buffer is filled. Files residing on disk are normally fully
 *    buffered by the standard I/O library.
 * 2. Line buffered. In this case, the standard I/O library performs I/O
 *    when a newline character is encountered on input or output. Line
 *    buffering is typically used on a stream when it refers to a terminal:
 *    standard input and standard output, for example.
 * 3. Unbuffered. The standard I/O library does not buffer the characters.
 *    The standard error stream, for example, is normally unbuffered.
 *
 * Most implementations default to the following types of buffering.
 * 1. Standard error is always unbuffered.
 * 2. All other streams are line buffered if they refer to a terminal
 *    device; otherwise, they are fully buffered.
 *
 * If we don't like these defaults for any given stream, we can change the
 * buffering by calling either of the following two functions.
 * #include <stdio.h>
 * void setbuf(FILE *restrict fp, char *restrict buf);
 * int setvbuf(FILE *restrict fp,char *restrict buf,int mode,size_t size);	
 *		Returns: 0 if OK, nonzero on error
 * 关于这两个函数的具体说明,可以看书中第5.4小节
 *
 * 这个例子是关于 行缓冲 的例子,全缓冲和无缓冲的情况应该是类似的.
 */
int main(void)
{
	char buf[BUFSIZZ] = {0};

	/* 将 stdout 的缓冲区设为buf数据,大小为BUFSIZZ,行缓冲模式 */
	if (setvbuf(stdout, buf, _IOLBF, BUFSIZZ) != 0)
		printf("setvuf error: %s\n", strerror(errno));

	/* 调用标准库函数fputs()打印字符串,由于字符串中不包含'\n',而 stdout 是
	 * 行缓冲,所以这个字符串并不会被立刻打印.
	 */
	fputs("tianxia", stdout);

	/* 将 buf 缓冲区中的从标准错误输出 stderr 中打出,由于 stderr 不缓冲,所以
	 * 即使存入buf缓冲区中的字符串 "tianxia" 并不包含'\n',也会立刻被打印出来
	 * 运行时,下面的字符串会比上面的字符串先打印出来,可见fputs()函数确实将
	 * 字符串还缓冲在buf里面,没有送到stdout打印出来.
	 */
	fprintf(stderr, "out to stderr: %s\n", buf);

	/* 调用fputs()函数打印带有'\n'字符的字符串,此时,下面的字符串会连同上面
	 * 的字符串一起打印出来.注意的是,这里调用了两次fputs()函数,这两次输入的
	 * 字符串在buf中分开存放的,"youqingren\n"顺序存放在"tianxia"后面,而不是
	 * 重叠的.我猜测是因为buf中的数据还没有被输出的缘故,下面对此有补充说明
	 */
	fputs("youqingren\n", stdout);

	/* 此时,虽然buf中的数据已经被输出到stdout,但是buf中的数据还在,如下.
	 * 这里,"also out to stderr: %s"字符串不需要在%s后面加'\n',因为buf中
	 * 包含了一个'\n'. 所以,在终端上输出这个字符串时,它会自己换行.
	 */
	fprintf(stderr, "also out to stderr: %s", buf);

	/* 此时,buf中的数据已经被输出过,再次调用fputs()函数往stdout输出字符串时,
	 * 新的字符串会覆盖之前的字符串,而不是顺序存放在之前的字符串后面.
	 * 书中5.2小节提到过,FILE结构体中包含a count of the number of characters
	 * currently in the buffer,可能当buf中的数据被输出时,这个count会被清零.
	 * 所以下次再往buf中写数据时,是从头开始写,导致出现覆盖的情况.
	 */
	fputs("new string\n", stdout);

	/* 行缓冲也带有全缓冲的特性: the size of the buffer that the standard
	 * I/O library is using to collect each line is fixed, so I/O might
	 * take place if we fill this buffer before writing a newline.
	 * 上面设置stdout的缓冲区大小为12,下面fputs()函数要输出的字符串长度大于
	 * 12,所以虽然这个字符串不带'\n',但是它的内容还是会立刻打印出来.
	 * 且奇怪的是,这个字符串会全部打印出来,不会发生截断.
	 */
	fputs("tianxiayouqingren", stdout);

	/* 作为对比,下面往stderr打印的字符串将会上面的字符串晚打印出来,而不是
	 * 像更上面的情况那样,往stderr打印的字符串先打印出来.
	 */
	fprintf(stderr, "\nthird out to stderr\n");

	return 0;
}
