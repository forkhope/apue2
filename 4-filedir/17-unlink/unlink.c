#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>

/* To remove an existing directory entry, we call the unlink() function.
 * #include <unistd.h>
 * int unlink(const char *pathname);
 *		Returns: 0 if OK, -1 on error.
 * The function removes the directory entry and decrements the link count
 * of the file referenced by pathname. If there are other links to the file,
 * the data in the file is still accessible through the other links. The
 * file is not changed if an error occurs.
 * We've mentioned before that to unlink a file, we must have write
 * permission and execute permission in the directory containing the
 * directory entry, as it is the directory entry that we will be removing.
 * 这里说的需要对目录拥有执行权限和写权限,仅仅指的是文件所在的目录,并不需要
 * 对目录的上级目录拥有写权限,例如要删除 dirparent/dirchild/tian 这个文件,
 * 至少需要对 dirchild 目录拥有执行和写权限,对 dirparent 目录拥有执行权限,
 * 不需要对 dirparent 目录拥有写权限,下面的代码中举例验证了这种情况.
 *
 * Only when the link count reaches 0 can the contents of the file be 
 * deleted. One other condition prevents the contents of a file from being
 * deleted: as long as some process has the file open, its contents will not
 * be deleted. When a file is closed, the kernel first checks the count of
 * the number of processs that have the file open. If this count has reached
 * 0, the kernel then checks the link count; if it is 0, the file's contents
 * are deleted. The property of unlink() is often used by a program to
 * ensure that a temporary file it creates won't be left around in case the
 * program crashes. The process creates a fil using either open() or creat()
 * and then immediately calls unlink(). The file is not deleted, however,
 * because it is still open. Only when the process either closes the file or
 * terminates, which causes the kernel to close all its open files, is the
 * file delted.
 *
 * If pathname is a symbolic link, unlink() removes the symbolic link, not
 * the file referenced by the link. There is no function to remove the file
 * referenced by a symbolic link given the name of the link.
 * 
 * The superuse can call unlink() with pathname specifying a directory, but
 * the function rmdir() should be used instead to unlink a directory.
 */
int main(void)
{
	struct stat buf;

	if (stat("tian", &buf) < 0) {
		printf("stat tian error: %s\n", strerror(errno));
		exit(1);
	}
	printf("%-10s %-10s\n", "i-node", "link count");
	printf("%-10ld %-10ld\n", (long)buf.st_ino, (long)buf.st_nlink);

	if (unlink("hlink-tian") < 0) {
		printf("unlink hlink-tian error: %s\n", strerror(errno));
		exit(1);
	}

	if (stat("tian", &buf) < 0) {
		printf("stat tian error: %s\n", strerror(errno));
		exit(1);
	}
	printf("after unlink hlink-tian:\n");
	printf("%-10s %-10s\n", "i-node", "link count");
	printf("%-10ld %-10ld\n", (long)buf.st_ino, (long)buf.st_nlink);

	printf("delete the slink-tian\n");
	if (unlink("slink-tian") < 0) {
		printf("unlink slink-tian error: %s\n", strerror(errno));
		exit(1);
	}

	printf("由于对目录 no-write 没有写权限,不能删除里面的文件\n");
	if (unlink("no-write/tian") < 0)
		printf("unlink no-write/tian error: %s\n", strerror(errno));

	printf("即使对目录 no-write2 没有写权限,由于对no-write2/execute-write"
		   "有执行和写权限,依然可删除no-write2/execute-write里面的文件\n");
	if (unlink("no-write2/execute-write/tian") < 0)
		printf("unlink no-write2/execute-write/tian error: %s\n",
				strerror(errno));
	else
		printf("unlink no-write2/execute-write/tian success...\n");

	exit(0);
}
