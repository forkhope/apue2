#include <stdio.h>

/* At any time, we can force a stream to be flushed.
 * #include <stdio.h>
 * int fflush(FILE *fp);
 * 		Returns: 0 if OK, EOF on error
 * This function causes any unwritten data for the stream to be passed to
 * the kernel. As a special case, if fp is NULL, this function causes all
 * output streams to be flushed.
 */
int main(void)
{
	/* 由于 stdout 是行缓冲,而下面的字符串中不包含'\n',所以这个字符串不会
	 * 立刻被打印出来.
	 */
	fputs("youqingren", stdout);

	/* stderr 是无缓冲,下面的字符串会立刻打印出来,即使它里面不包含'\n' */
	fputs("tianxia :", stderr);

	/* 调用 fflush() 函数强制冲洗 stdout,它里面缓冲的字符串会打印出来. */
	fflush(stdout);

	fputs("\n", stdout);
	return 0;
}
