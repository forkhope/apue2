#include "apue.h"
#include <fcntl.h>

/* fcntl(): Get/set file status flags (cmd = F_GETFL or F_SETFL)
 * F_GETFL	Return the file status flags for filedes as the value of
 * 			function. They are: O_RDONLY, O_WRONLY, O_RDWR, O_APPEND, 
 *			O_NONBLOCK, O_SYNC, O_DSYNC, O_RSYNC, O_FSYNC, O_ASYNC.
 * F_SETFL	Set the file status flags to the value of the third argument.
 *			The only flags that can be changed are O_APPEND, O_NONBLOCK,
 *			O_SYNC, O_DSYNC, O_RSYNC, O_FSYNC, O_ASYNC. Linux man 手册提到
 *			如果指定了F_SETFL,则使用下面的 标志位 作为第三个参数时会被忽略
 *			O_RDONLY, O_WRONLY, O_RDWR, O_CREAT, O_EXCL, O_NOCTTY, O_TRUNC
 *			即这些标志位不能通过 fcntl() 函数来设置.
 */
int main(int argc, char *argv[])
{
	int val;

	if (argc != 2)
		err_quit("Usage: fcntl-fl <descriptor#>");

	// F_GETFL: Read the file status flags; arg(指第三个参数) is ignored.
	if ((val = fcntl(atoi(argv[1]), F_GETFL, 0)) < 0)
		err_sys("fcntl-fl: error for fd %d", atoi(argv[1]));
	printf("file status flag = %x\n", val);

	/* 通过打印出这些标志宏的值,并和上面打印出的 val 值进行对比,可以
	 * 更好观察到下面一些判断语句的真假情况.
	 */
	printf("O_ACCMODE = %x\n", O_ACCMODE);	// 打印出的值为 3
	printf("O_RDONLY = %x\n", O_RDONLY);	// 打印出的值为 0 
	printf("O_WRONLY = %x\n", O_WRONLY);	// 打印出的值为 1	
	printf("O_RDWR = %x\n", O_RDWR);		// 打印出的值为 2
	printf("O_APPEND = %x\n", O_APPEND);	// 打印出的值为 400
#if defined(O_SYNC)
	printf("O_SYNC = %x\n", O_SYNC);		// 打印出的值为 1000
#endif

	/* We must first use the O_ACCMODE mask to obtain the access-mode bits
	 * and then compare the result against any of the three values.
	 */
	switch (val & O_ACCMODE) {
		case O_RDONLY:
			printf("read only");
			break;
		case O_WRONLY:
			printf("write only");
			break;
		case O_RDWR:
			printf("read write");
			break;
		default:
			err_quit("fcntl-fl: unknown access mode");
			break;
	}

	if (val & O_APPEND)
		printf(", append");
	if (val & O_NONBLOCK)
		printf(", nonblocking");
#if defined(O_SYNC)
	if (val & O_SYNC)
		printf(", sync");
#endif
	putchar('\n');
	exit(0);
}
