#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

/* Write your own dup2() function that performs the same service as the
 * dup2() function described in Section 3.12, without calling the fcntl()
 * function. Be sure to handle errors correctly.
 */
int dup2_l(int oldfd, int newfd)
{
	/* 这里假设一个进程最大可以打开的文件描述符总数是 256, 书中提供了
	 * 一个函数 open_max(), 可以较精确地得到文件描述符总数的真实值.
	 */
	int fd[256] = {0};
	int i;

	if (newfd >= 256 || newfd < 0)
		return -1;

	/* 判断 oldfd 是否是有效的文件描述符.
	 * 这里不要用 dup() 函数来判断,因为可能存在进程中的所有文件描述符都在
	 * 打开的情况,此时用 dup() 函数来判断,会报 EMFILE 错.
	 *
	 * 注意,也不能使用 read() 或者 write() 函数, 使用 write() 函数可能会
	 * 改变文件内容; 使用 read() 函数会改变文件指针偏移,对 oldfd 文件描述
	 * 符造成影响. 下面试图使用 lseek(oldfd, 0, SEEK_CUR) 语句来作判断,这
	 * 条语句对 oldfd 文件描述符应该不会造成其他影响.
	 */
	if (lseek(oldfd, 0, SEEK_CUR) < 0 && errno == EBADF) {
		printf("file descriptor: %d is not valid, %m\n", oldfd);
		return -1;
	}

	/* 如果 oldfd 是有效的文件描述符,判断 oldfd 和 newfd 是否相等.
	 * 如果相等,则直接返回 newfd, 此时不会关闭 newfd.
	 */
	if (oldfd == newfd)
		return newfd;

	/* 如果 oldfd 有效,且 oldfd 和 newfd 不相等,则关闭 newfd; 这里不判断
	 * newfd 是否已经打开,如果 newfd 已经打开,则 close() 函数执行成功,如果
	 * newfd 没有打开,则 close() 函数执行失败,但这没有影响.
	 */
	close(newfd);	/* 没有判断 close() 被中断的情况 */

	for (i = 0; i < 256; ++i) {
		if ((fd[i] = dup(oldfd)) < 0) {
			printf("dup: %s\n", strerror(errno));
			while (--i > 0)
				close(fd[i]);
			return -1;
		}

		if (fd[i] == newfd)
			break;
	}

	/* 这种情况可能不会发生 */
	if (i >= 256)
		return -1;

	while (--i >= 0)
		close(fd[i]);

	return newfd;
}
