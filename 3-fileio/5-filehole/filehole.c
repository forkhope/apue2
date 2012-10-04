#include "apue.h"
#include <fcntl.h>

char buf1[] = "abcdefghij";
char buf2[] = "ABCDEFGHIJ";

/* Create a file with a hole in it */
int main(void)
{
	int fd;
	char buf[1024];

	if ((fd = open("file.hole", O_RDWR | O_CREAT, 0644)) < 0)
		err_sys("filehole: cant' create a new file");
	else
		printf("filehole: create a new file (file.hole) SUCCESS\n");

	if (write(fd, buf1, 10) != 10)
		err_sys("filehole: can't write to the new file");

	/* The file's offset can be greater than the file's current size, in
	 * which case the next write() to the file will extend the file. This
	 * is referred to as creating a hole in a file and is allowed. Any 
	 * bytes in a file that have not been written are read back to 0.
	 */
	if (lseek(fd, 1000, SEEK_SET) == -1)
		err_sys("filehole: lseek error");

	/* 根据上面的描述,此次写入的内容与第一次写入的内容之间隔有很多个0 */
	if (write(fd, buf2, 10) != 10)
		err_sys("filehole: can't write to the new file");

	/* 先将 "current file offset" 设为文件开始处 */
	if (lseek(fd, 0, SEEK_SET) == -1)
		err_sys("filehole: can't lseek to the beginning of file");

	/* 从文件开始处读取文件内容,由于中间隔有0,使用 read() 函数可以读出这些
	 * 0,由于'\0'字符的整数值就是0,如果使用标准I/O函数来操作读取出的内容,
	 * 则 0 后面的字符全都无法操作,因为大部分标准I/O函数遇到'\0'就终止了.
	 */
	if (read(fd, buf, 1024) < 0)
		err_sys("filehole: can't read from file.hole");
	// 0 后面的内容将无法通过 printf() 函数来打印出来.
	printf("第一次读到的buf = %s\n", buf);

	/* 再将 "current file offset" 设到1000处,跳过中间的 0 */
	if (lseek(fd, 1000, SEEK_SET) == -1)
		err_sys("filehole: lseek error");

	// 由于中间的 0 已经全部被跳过,可以用 printf() 函数打印出 0 后面的内容
	if (read(fd, buf, 1024) < 0)
		err_sys("filehole: can't read from file.hole");
	printf("第二次读到的buf = %s\n", buf);

	exit(0);
}
