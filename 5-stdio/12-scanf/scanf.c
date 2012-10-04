#include <stdio.h>

/* Formatted input is handled by the three scanf() functions.
 * #include <stdio.h>
 * int scanf(const char *format, ...);
 * int fscanf(FILE *fp, const char *format, ...);
 * int sscanf(const char *buf, const char *format, ...);
 * 		All three return: number of input items assigned, EOF if input
 * 						  error or end of file before any conversion
 * 						  
 * The scanf() family is used to parse an input string and convert character
 * sequences into variables of specified types. The arguments following the
 * format contain the addresses of the variables to initialize with the
 * results of the conversions.
 *
 * The format specification controls how the arguments are converted for
 * assignment. The percent sign (%) indicates the start of a conversion
 * specification. Except for the conversioin specifications and white space,
 * other characters in the format have to match the input. If a character
 * doesn't match, processing stops, leaving the remainder of the input
 * unread. There are three optional components to a conversion
 * specification, shown in square brackets below:
 * 		%[*][fldwidth][lenmodifier]convtype
 *
 * As with the printf family, the scanf family also supports functions that
 * use variable argument lists as specified by <stdarg.h>
 * vscanf(), vfscanf(), vsscanf()
 */
int main(void)
{
	int i = 7;
	char buf[16] = "3:5:6 8/9/2";
	int a, b, c, d, e, f;

	/* The optional leading asterisk [*] is used to suppress conversion.
	 * Input is converted as specified by the rest of the conversion
	 * specification, but the result is not stored in an argument.
	 */
	scanf("%*d", &i);       // scanf()使用了 %*d, 则 i 的值并没有改变
	printf("i = %d\n", i);  // 打印出来 i 的值还是 7.

	/* 这里不要写为 %d\n,否则只按一次回车,无法结束输入.如果想按一次回车,
	 * 就结束输入,只写 %d 即可.
	 */
	scanf("%d", &i);
	printf("i = %d\n", i);

	sscanf(buf, "%d:%d:%d %d/%d/%d", &a, &b, &c, &d, &e, &f);
	printf("%d:%d:%d %d/%d/%d\n", a, b, c, d, e, f);

	return 0;
}
