#include <stdio.h>

/* Formatted output is handled by the four printf functions.
 * #include <stdio.h>
 * int printf(const char *fmt, ...);
 * int fprintf(FILE *fp, const char *fmt, ...);
 * 		Both return: number of characters output if OK, negative value if
 * 					 output error
 * int sprintf(char *buf, const char *format, ...);
 * int snprintf(char *buf, size_t n, const char *format, ...);
 * 		Both return: number of characters stored in array if OK, negative
 * 					 value if encoding error.
 * The printf function wirtes to the standard output, fprintf writes to the
 * specified stream, and sprintf places the formatted characters in the
 * array buf. The sprintf function automatically appends a null byte at the
 * end of the array, but this null byte is not included in the return value
 *
 * The functions snprintf() and vsnprintf() do not write more than n bytes
 * (include the trailing '\0').也就是说,如果第二个参数是5,表示最多5个字符,
 * 但实际上,实际转换的有效字符是4个,因为snprintf()默认会添加一个'\0'.这个最
 * 多转换的字符个数包含了'\0'这个字符.
 * 如果所给字符串长度大于或者等于第二个参数n, snprintf()函数最多转化n-1个
 * 字符,并将buf[n-1]置为'\n'.即,对于数组buf[5], 执行完下面的语句后:
 * 	snprintf(buf, 5, "tianx")
 * buf中保存的是"tian",且buf[4]是'\0'.
 *
 * A conversion specification has four optional component, shown in square
 * brackets: %[flags][fldwidth][precission][lenmodifier]convtype
 * [flags]的可选值有5个: -, +, (space), #, 0; (space)是指空格.
 * 下面的程序给出这些选项的说明.使用的时候一些要注意的地方可以查看man手册.
 *
 * ---------------------------------------------------------------------
 * The following four variants of the printf family are similar to the
 * previous four, but the variable argument list (...) is replaced with arg
 * #include <stdarg.h>
 * #include <stdio.h>
 * int vprintf(const char *format, va_list arg);
 * int vfprintf(FILE *fp, const char *format, va_list arg);
 * 	Both return: number of characters output if OK, negative value if error
 * int vsprintf(char *buf, const char *format, va_list arg);
 * int vsnprintf(char *buf, size_t n, const char *format, va_list arg);
 *  	Both return: number of characters stored in array if OK,
 *					 negative value if encoding error
 * --------------------------------------------------------------------
 */
int main(void)
{
	int i = 8, j = -9;
	char buf[10];
	long k = 123456789L;
	double d = 879.38348, g1, g2;

	/* C语言中常量值默认是一个32位的有符号整型数,下面如果写成:
	 * 		unsigned int u = 2147483650;
	 * 编译告警:warning: this decimal constant is unsigned only in ISO C90
	 * 解决方法有两种:
	 * (1)添加后缀 u 或者 U 进行强制类型转换,将常量值转换为无符号整型数.
	 *    即写成 unsinged int u = 2147483650u;
	 * (2)使用C99标准来编译.即增加编译选项 -std=c99
	 *
	 * K&R 书中2.3 Constants小节开篇就提到这一点: An integer constant like
	 * 1234 is an int. A long constant is written with a terminal l or L,
	 * as in 123456789L; Unsigned constants are written with a terminal u
	 * or U, and the suffix ul or UL indicates unsigned long.
	 */
	unsigned int u = 2147483650u;

	/* [flags]是+时: always disply sign of a signed conversion.
	 * [flags]是 时(即空格): prefix by a space if no sign is generated.
	 * 当 + 和 ' '同时出现时, ' '会被忽略,即只有 + 有作用.
	 * 下面语句的执行结果是: i=+8, j=-9; i= 8, j=-9
	 */
	printf("i=%+d, j=%+d; i=% d, j=% d\n", i, j, i, j);

	/* [flags]是#时: convert using alternate form (include 0x prefix
	 * for hex format, for example)
	 * 下面的%d表示打印十进制,%x表示打印十六进制,但是不带0x前缀,%#x表示打印
	 * 十六进制,且带0x前缀. 当打印八进制时,格式类似,八进制的前缀是0.
	 */
	printf("0x780=%d, 0x780=%x, 0x780=%#x\n", 0x780, 0x780, 0x780);
	printf("0745=%d, 0745=%o, 0745=%#o\n", 0745, 0745, 0745);

	/* The [fldwidth] component specifies a minimum field width for the
	 * conversion. If the conversion results in fewer characters, it is
	 * padded with spaces. The field width is a non-negative decimal
	 * integer or an asterisk.
	 *
	 * 当转换之后的长度小于所给域宽[fidwidth]时,默认以空格填充空出来的部分,
	 * 并右对齐,即用空格填充的是左边空出来的部分,可以用[flags]来改变这一点.
	 * [flags]是-时: left-justify the output in the field; 表示左对齐.
	 * [flags]是0时: prefix with leading zeros instead of padding with 
	 * spaces,实际测试时,这个选项只对数字转换有用,它不会在字符串和字符的
	 * 转换之前自动加0. 且 %08s 和 %08c 在编译时会告警:
	 * 		warning: '0' flag used with ‘%s’ gnu_printf format
	 *
	 * 当[flags]的 - 和 0 混用时,只有 - 会生效, 0 没有起到作用,即无论是
	 * %0- 还是 %-0 都只会是左对齐,它们不会在右边空出来的地方加 0,因为0这个
	 * 选项只是用于前缀(prefix),当左对齐时,不存在前缀填充(prefix with lead)
	 * 且当这两个选项混用时,编译会告警:
	 * 		warning: '0' flag ignored with '-' flag in gnu_printf format
	 *
	 * 上面提到,[fidwidth]的值是一个非负整数,实际上,我们无法提供一个负的
	 * 域宽值或者浮点数的域宽值.例如 %-8 会被解析为[flags]是-,[fidwitdh]是8
	 * 这里的 -8 并不会被认为是负数8.同理, %0.8 会被解析为[flags]是0,此时,
	 * 不存在[fidwidth], .8 对应的是[precision].因为[fidwith]和[precision]
	 * 都是整数值,当只存在其中一个时,为了区分它们,[precision]要求以点(.)开
	 * 头.即, %8d 表示[fidwidth]是8, %.8d 表示[precision]是8.
	 */
	printf("%8d\n%8d\n%8s\n%8c\n", 4808, 583482, "tian", 'a');
	printf("%08d\n%08d\n%8s\n%8c\n", 4808, 583482, "tian", 'a');
	printf("%-8d\n%-8d\n%-8s\n%-8c\n", 4808, 583482, "tian", 'a');

	/* The [precision] component specifies the minimum number of digit to
	 * appear for integer conversions, the minimum(ERROR) number of digits 
	 * to appear to the right of the decimal point for floating-point
	 * conversions, or the maximum number of bytes for string conversion.
	 * The precision is a period (.) followed by a optional non-negative
	 * decimal integer or an asterisk.
	 *
	 * [precission]的作用和将[flags]设为0,然后指定[fldwidth]的作用差不多,
	 * 如果将[flags]设为0,再指定[precision],编译时会告警:
	 * warning: '0' flag ignored with precision and ‘%d’ gnu_printf format
	 * 下面语句的执行结果是:0083, 0083, 38984
	 * 即, %.4d 和 %04d 的结果差不多.
	 *
	 * 当转换整数时,如果整数的位数不够,会在前面补0;如果整数位数超过,不会发生
	 * 截断,而是全部打印出来; 对于整数来说,默认的[precision]是1.
	 */
	printf("%.4d, %04d, %.4d\n", 83, 83, 38984);

	/* 书中提到,当转换浮点数时,[precision]表示小数点之后的最小个数.这个描述
	 * 应该是错误的.[precision]表示的是小数点之后的个数,刚好就是这个值,如果
	 * 小数点个数不够,会用0填充;如果小数点个数超过,会四舍五入,只保留指定个数
	 * 下面语句的执行结果是: 83.4899, 83.4894, 83.4500, 83.450000
	 * 对于浮点数来说,默认的[precision]是6.所以打印的第四个值是 83.450000
	 */
	printf("%.4f, %.4f, %.4f, %f\n", 83.48989, 83.48942, 83.45, 83.45);

	/* 当转换字符串时,[precision]表示最大打印的字符数 */
	printf("%.5s, %.5s\n", "tianxia", "tian");

	/* Both the field width and precision can be an asterisk. In this case,
	 * an integer argument specifies the value to be used. The argument
	 * appears directly before the argument to converted.
	 */
	printf("%*.*s\n", 10, 5, "tianxia");

	/* 可以使用 %.*s 来作为 sprintf() 函数的转换格式,起到和 snprintf() 类似
	 * 的功能.要注意的是, sprintf()会自动追加'\0'到转换后的字符串里面,但是
	 * 这个'\0'没有被计算在 sprintf() 的返回值中.由于buf的数组大小是10,所以
	 * 下面转换的最大字符个数应该是9,空出来最后一个元素来保存sprintf()追加的
	 * '\0'.而 snprintf() 应该使用 10 来作为最大转换的字符个数,因为这个函数
	 * 最多转换n-1个字符,并将buf[n-1]置为'\0'.
	 */
	i = sprintf(buf, "%.*s", 9, "tianxiayouqingren");
	if (buf[9] == '\0') // %.9s 格式化后,最大的字符个数是 9,所以下面的i是9
		printf("sprintf: i=%d,%s\n", i, buf);
	/* 下面的语句只会转换"tianxiayo"这九个字符到buf中,然后buf[9]=='\0'.
	 * sprintf() 和 snprintf() 都返回格式化转后的字符个数(不包含'\0').但是,
	 * snprintf()函数返回的字符个数并不等于它实际转换到buf中的字符个数.如,
	 * 下面的第一次转换,返回10,因为"tianxiayou"的长度是10(不包含'\0',下同);
	 * 第二次转换,返回7,因为"tianxia"的长度是7;第三次转换,返回17,因为"tian
	 * xiayouqingren"的长度是17.当snprintf()函数的返回值等于或大于 n 时,表示
	 * 发生了截断,如果小于 n, 那就是没有发生截断.
	 */
	i = snprintf(buf, 10, "%s", "tianxiayou");
	if (buf[9] == '\0')
		printf("first: i=%d,%s\n", i, buf);	// i 的值是 10
	i = snprintf(buf, 8, "%s", "tianxia");
	printf("second: i = %d,%s\n", i, buf);	// i 的值是 7
	i = snprintf(buf, 8, "%s", "tianxiayouqingren");
	printf("third: i = %d,%s\n", i, buf);   // i 的值是 17
	/* 虽然上面的语句格式化后的字符个数是17,且snprintf()返回的也是17,但实际
	 * 它并没有真的写入17个字符,从上面的打印结果可以看出,buf中存的是tianxia
	 * 此时,buf[7]被自动追加为'\0',而buf[8]则保持不变,还是第一次写入的'o'.
	 */
	printf("buf[8]: %c\n", buf[8]); // 打印结果是 'o'

	/* The [lenmodifier] component specifies the size of the argument.
	 * 转换长整型时使用的 %ld 中的 l 就是 [lenmodifier] 的值.
	 */
	printf("k = %ld\n", k);

	/* The convtype component is not optional. It controls how the
	 * argument is interpreted.
	 */
	printf("i = %d, j = %i\n", i, j); // %d 和 %i 都表示打印有符号十进制值

	/* %d 和 %o 的区分是:使用 %d 转换后的值是 signed decimal.不管所给的值
	 * 是十进制的值,八进制的值,十六进制的值,打印出来的值都是十进制的值.因为
	 * 代码里面写的值最终编译之后,保存在二进制文件中的都是二进制的值,0x10和
	 * 16 和 020 对应的都是二进制 00010000. %d 将 00010000 显示为十进制的16
	 * 并打印出来. 而 %o 转换后的值是 unsigned octal, %o 打印出来的值就是八
	 * 进制的值,虽然默认情况下,打印出来的值前面并没有加0来表示这是个八进制
	 * 的值,但它确实是一个八进制的值.
	 */
	printf("%%d: 0345 = %d,", 0345); // 打印 229, 0345转换为十进制是 229
	printf("%%o: 0345 = %o,", 0345); // 打印 345, 0345对应的八进制就是 345
	printf("%%o: 345  = %o\n", 345); // 打印 531, 345转换为八进制是 531
	printf("%%x: 0x345 = %x, %X\n", 0x345, 345); // 十六进制的情况类似
	/* %x 和 %X 的区分是: the letters abcdef are used for x conversions;
	 * the letters ABCDEF are used for X conversions.
	 */
	printf("%%x: 0xabc = %x, %%X: 0xabc = %X\n", 0xabc, 0xabc);

	printf("u = %u\n", u); // 注意变量 u 的初始化值的写法,要加后缀u

	/* %f, %e和 %g 的区分是: 对于%f, %e来说,[precision]默认是6,指的是
	 * 小数部分的精度; 对于%g, %G来说,[precision]默认也是6,但是它指的是
	 * 整数部分和小数部分的有效位数.
	 * 下面变量d 的初始值是 879.38348, 使用 %f 打印出来是 879.383480,
	 * 使用 %g 打印出来是 879.383, 默认只保留6位有效位.
	 * 使用 %e 打印出来是 8.793835e+02,小数部分只保留6位精度,并进行四舍五入
	 */
	printf("d = %e, %E\n", d, d);
	printf("d = %f, %F\n", d, d);
	printf("d = %g, %G\n", d, d);

	/* Style e is used if the exponent from its conversion is less than -4
	 * or greater than or equal to the precision.
	 * 下面的语句打印出来是: g1 = 4.938e-05, g2 = 1.80848E+08
	 * 当使用自然记数法转换后,指数小于-4,大于或者等于[precision]时,%g, %G
	 * 使用 %e, %E 来打印浮点数. 这就是%g什么时候用%f,什么时候用%e的规则.
	 */
	g1 = 0.00004938;
	g2 = 180848389.5;
	printf("g1 = %g, g2 = %G\n", g1, g2);

	/* %a, %A: double floating-point number in hex exponential format */
	printf("g2 = %a, g1 = %A", g2, g1);
	printf("%c", '\n'); /* character (with l length modifier, wide char) */
	printf("%%s: %s\n", "string(with l length modifier,wide char string)");

	printf("&d = %p\n", &d); /* %p: pointer to a void */
	printf("使用%%%%来打印百分号: %%\n"); /* %%: a % character */

	/* %n: pointer to a signed integer into which is which is written the
	 * number of characters written so far.
	 * %n 将直到%n为止,目前已经转换的字符个数存入%n所对应的指针参数指向的
	 * 有符号整型变量中.下面的i将会被赋值为7,因为在%n之前转换了7个字符.
	 */
	printf("tianxia%n", &i);
	j = printf(", i = %d\n", i);
	printf("j = %d\n", j); // 最后,printf()函数返回成功转换的字符个数.

	return 0;
}
