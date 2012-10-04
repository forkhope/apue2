#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

/* Every open file has an associated "current file offset", measures the
 * number of bytes from the beginning of the file.
 * An open file's offset can be set explicitly by calling lseek().
 * #include <unistd.h>		// 下面的 off_t 是有符号型的整数
 * off_t lseek(int filedes, off_t offset, int whence);
 * Return: new file offset if OK, -1 on error.
 *
 * The interpretation of the offset depends on the value of the whence.
 * If whence is SEEK_SET, the file's offset is set to offset bytes from
 * 		the beginning of the file.	
 * If whence is SEEK_CUR, the file's offset is set to its current value 
 *		plus the offset. The offset can be positive or negative.	
 * If whence if SEEK_END, the file's offset is set to the size of the file 
 *		plus the offset. The offset can be positive or negative.
 */
int main(void)
{
	int fd;
	off_t pos;
	char buf[12];
	
	if ((fd = open("a.txt", O_RDWR | O_CREAT, 0644)) < 0) {
		printf("lseek: can't create a new file (a.txt): %s\n", 
				strerror(errno));
		return 1;
	}

	/* By default, this offset if initialized to 0 when a file is opened,
	 * unless the O_APPEND option is specified. 下面的结果将证实这一点.
	 * 
	 * Because a successful call to lseek() returns the new file offset,
	 * we can seek zero bytes from the current position to determine the
	 * current offset: off_t currpos = lseek(fd, 0, SEEK_CUR);
	 */
	if ((pos = lseek(fd, 0, SEEK_CUR)) == -1) {
		printf("lseek: can't lseek the a.txt: %s\n", strerror(errno));
		return 1;
	}
	printf("When a file is opened, offset is %ld\n", pos);

	/* 如果写为 write(fd, "tianxia", 7), 则"tianxia"最后的'\0'没有被写入,
	 * 如果写为 write(fd, "tianxia", 8), 则"tianxia"最后的'\0'也被写入,
	 * 这会造成一定的影响,详见 write 函数对应的示例: 7-write/write.c
	 */
	if (write(fd, "tianxia", 8) != 8) {
		printf("lseek: can't write to a.txt: %s\n", strerror(errno));
		return 1;
	}

	/* Read and write operations normally start at the current file offset
	 * and cause the offset to be incremented by the number of bytes read
	 * or written. 下面的例子用于证实这一点.
	 */
	if ((pos = lseek(fd, 0, SEEK_CUR)) == -1) {
		printf("lseek: can't lseek the a.txt: %s\n", strerror(errno));
		return 1;
	}
	printf("After write 8 bytes, offset is %ld\n", pos);

	if ((pos = lseek(fd, 4, SEEK_SET)) == -1) {
		printf("lseek: can't lseek the a.txt: %s\n", strerror(errno));
		return 1;
	}
	printf("调用lseek(fd, 4, SEEK_SET)后,read()函数将会从第四个字节读起\n");
	if (read(fd, buf, 4) < 0) {
		printf("lseek: can't read from a.txt: %s\n", strerror(errno));
		return 1;
	}
	printf("此时,调用read()函数读到的内容为: %s\n", buf);

	if ((pos = lseek(fd, 0, SEEK_END)) == -1) {
		printf("lseek: can't lseek the a.txt: %s\n", strerror(errno));
		return 1;
	}
	printf("lseek(fd, 0, SEEK_END)返回文件末尾的偏移: %ld\n",pos);

	/* This technique(指 lseek(fd, 0, SEEK_CUR) can also be used to
	 * determine if a file is capable of seeking. If the file descriptor
	 * refers to pipe, FIFO, or socket, lseek sets errno to ESPIPE and
	 * returns -1.
	 */
	if (lseek(STDIN_FILENO, 0, SEEK_CUR) == -1)
		printf("lseek: cannot seek\n");
	else
		printf("lseek: seek OK\n");

	return 0;
}
