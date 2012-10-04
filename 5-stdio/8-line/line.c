#include <stdio.h>
#include <string.h>

/* Line-at-a-time input is provided by the following two functions.
 * #include <stdio.h>
 * char *fgets(char *restrict buf, int n, FILE *restrict fp);
 * char *gets(char *buf);
 * 		Both return: buf if OK, NULL on end of file or error
 * With fgets, we have to specify the size of the buffer, n. This function
 * reads up through and including the next newline, buf no more than n-1
 * characters, into the buffer. The buffer is terminated with a null byte.
 * The gets function should never be used.
 *
 * Line-at-a-time output is provided by fputs() and puts().
 * #include <stdio.h>
 * int fputs(const char *restrict str, FILE *restrict fp);
 * int puts(const char *str);
 * 		Both return: non-negative value if OK, EOF on error.
 * The function fputs writes the null-terminated string to the specified
 * stream. The null byte at the end is not written. Note that this need not
 * be line-at-a-time output, since the string need not contain a newline as
 * the last non-null-character.
 * The puts function writes the null-terminated string to the standard
 * output, without writing the null byte.
 */
int main(void)
{
	char buf[12] = {0};

	/* fgets()函数从指定的 stream 中获取到最多n个字符的字符串,由于fgets()会
	 * 保证这个字符串是以'\0'结尾的,所以实际上,它最多读取n-1个字符,以空出
	 * 一个位置来保存'\0',此时, buf[n-1] == '\0',而不是 buf[n] == '\0'.
	 * 下面,在运行的时候,可以输入一个长度大于12的字符串,然后检查buf[11]的值
	 *
	 * 且此时要注意的是,由于输入的字符串长度大于12,则输入的'\n'也在超出的
	 * 长度范围之外,'\n'并没有被保存到buf中,fgets()函数只保证字符串是以'\0'
	 * 结尾,并不保证'\0'的前一个字符一定是'\n',即fgets()函数获取到的字符串
	 * 并不一定包含'\n'.如果是从终端获取输入,由于终端输入的时候,是要按回车来
	 * 表示一次输入的结束,当终端输入的字符串长度小于n时,回车就包含在里面,所
	 * 以从终端输入时,看起来fgets()总是返回'\n'一样,其实不是,只是因为从终端
	 * 输入时,每次都要输入'\n',fgets()才能经常读到'\n'(在长度不够的时候).
	 *
	 * 对 fgets(buf, 12, stdin) 语句来说, 它最多读取 11 个字符,如果输入的
	 * 字符个数多于11,则后面的所有字符并不被保存到buf里面,此时, buf[11]被
	 * 置为'\0',对于数组来说,buf[11]就是数组的第12个元素.
	 */
	fgets(buf, 12, stdin);
	if (buf[11] == '\0')
		printf("buf[11] == 0\n");
	else
		printf("buf[11] != 0\n");

	/* 这个时候, buf 中保存的字符串长度是 11, buf是一个 12 的数组,本来可以
	 * 保存 12 个字符,但由于 fgets() 保留了最后一个位置来存放 '\0', 而'\0'
	 * 是不包含了字符串长度里面的,所以此时, buf 的长度是 11.
	 */
	printf("size of buf is: %d\n", strlen(buf));

	/* fgets()获取的字符串不一定包含'\n', fputs()也类似,fputs()函数输出的
	 * 字符串也不一定就自动换行,如果字符串中包含了'\n',则fputs()打印的字符串
	 * 会换行,如果不包含'\n',则fputs()打印的字符串不换行.即:
	 * fputs()不会在所给字符串后面追加'\n',如果fgets()如果读不到'\n',也不会
	 * 往缓冲区中追加'\n'一样.
	 */
	fputs(buf, stdout);

	/* 不同于 fputs(), puts()函数在打印完所给字符串后,会自动换行.即,如果所给
	 * 字符串中已经包含了'\n', 则调用puts()函数会换行两次,如下所示:
	 */
	puts("\nEnd.....\n");
	return 0;
}
