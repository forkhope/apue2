#include "apue.h"
#include <fcntl.h>

int main(void)
{
	int fd, n;
	char buf[16];

	if ((fd = open("new", O_RDWR | O_CREAT, 0644)) < 0)
		err_sys("use_dup2: can't open or create the file (new)");

	if (dup2_l(fd, 4) < 0)
		err_sys("use_dup2: can't dup2_l the file descriptor: %d", fd);

	if (write(4, "tian", 4) != 4)
		err_sys("use_dup2: can't write 'tian' to file (new)");

	if (write(fd, "xia", 3) != 3)
		err_sys("use_dup2: can't write 'xia' to file (new)");

	/* 在写代码时,没有注意到文件指针偏移的情况,直接使用 read() 函数读,
	 * 返回的是 0,读到EOF,这是因为执行write()函数后,文件指针偏移,此时,
	 * 指向的正是文件末尾. 这种情况遇到多次,对此要有清醒的认识.
	 * 可以使用lseek()函数来改变文件指针的值,如下.
	 */
	if (lseek(fd, 0, SEEK_SET) < 0)
		err_sys("use_dup2: cant' lseek the file (new)");

	if ((n = read(fd, buf, 16)) < 0)
		err_sys("use_dup2_l: can't read from the file (new)");
	buf[n] = '\0';
	printf("buf: %s\n", buf);

	if (close(fd) < 0)
		err_sys("use_dup2: can't close the file descriptor: %d", fd);
	if (close(4) < 0)
		err_sys("use_dup2: can't close the file descriptor: 4");

	exit(0);
}
