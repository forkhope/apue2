#include <wchar.h>		/* fwide() */
#include <stdio.h>
#include <string.h>
#include <errno.h>

/* 对于 orientation 的概念,可以看书中第 5.2 小节.
 * fwide - set and determine the orientation of a FILE stream.
 * #include <wchar.h>
 * int fwide(FILE *stream, int mode);
 * 		Returns: positive if stream is wide-oriented,
 * 				 negative if stream is byte-oriented,
 * 				 or 0 if stream has no orientation.
 * The fwide() function performs different tasks, depending on the value
 * of the mode argument.
 * (1) If the mode argument is negative, fwide() will try to make the
 *     specified stream byte-oriented.
 * (2) If the mode argument is positive, fwide() will try to make the
 *     specified stream wide-oriented.
 * (3) If the mode arugment is zero, fwide() will not try to set the
 *     orientation, but will still return a value identifying the stream's
 *     orientation.
 * Note that fwide() will not change the orientation of a stream that is
 * already oriented.
 */
int main(void)
{
	FILE *fp;
	int orientation;

	if ((fp = fopen("tian", "r")) == NULL)
		printf("fopen Makefile error: %s\n", strerror(errno));

	/* Initially, when a stream is created, it has no orientation. */
	/* fwide()函数是 C99 的标准,仅仅包含 <wchar.h> 头文件,使用 gcc -o 来编译
	 * 的时候还是会告警,说是"implicit declaration of function fwide",需要
	 * 增加编译选项 -std=c99 来指定使用 C99 标准,从而去掉这个告警.
	 */
	orientation = fwide(fp, 0);
	printf("刚调用fopen(),The orientation is: %d\n", orientation);

	/* If a multibyte I/O function (see <wchar.h>) is used on a stream
	 * without orientation, the stream's orientation is set to wide-oriented
	 * If a byte I/O function is used on a stream without orientation, the
	 * stream's orientation is set to byte-oriented.
	 */
	fputc('A', fp);

	orientation = fwide(fp, 0);
	printf("调用fputc(),The orientation is: %d\n", orientation);

	printf("调用fwide(fp,1)试图将fp改为wide-oriented:");
	fwide(fp, 1);
	printf("%d\n", fwide(fp, 0));

	fclose(fp);

	return 0;
}
