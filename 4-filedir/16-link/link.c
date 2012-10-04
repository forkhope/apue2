#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

/* As we saw in the previous section, any file can have multiple directory
 * entries pointing to its i-node. The way we create a link to an existing
 * file is with the link() function.
 * #include <unistd.h>
 * int link(const char *oldpath, const char *newpath);
 *		Returns: 0 if OK, -1 on error.
 * link() creats a new link (also known as a hard link) to an existing file
 * If the newpath already exists, an error is returned. Only the last
 * component of the newpath is created. The rest of the path must already
 * exist. The creation of the new directory entry and the increament of the
 * link count must be an atomic operation.
 * Most implementations require that both pathnames be on the same file
 * system, although POSIX.1 allows an implementation to support linking
 * across file systems.
 */
int main(void)
{
	int fd;
	struct stat buf;

	if ((fd = open("tian", O_RDWR | O_CREAT, 0644)) < 0) {
		printf("create tian error: %s\n", strerror(errno));
		exit(1);
	}

	if (stat("tian", &buf) < 0) {
		printf("stat tian error: %s\n", strerror(errno));
		exit(1);
	}

	printf("information of tian:\n");
	printf("%-10s %-10s\n", "i-node", "link count");
	printf("%-10ld %-10ld\n", (long)buf.st_ino, (long)buf.st_nlink);

	/* 创建一个链接到 tian 的硬链接文件 hlink-tian */
	if (link("tian", "hlink-tian") < 0) {
		printf("link tian to hlink-tian error: %s\n", strerror(errno));
		exit(1);
	}

	if (stat("tian", &buf) < 0) {
		printf("stat tian error: %s\n", strerror(errno));
		exit(1);
	}
	printf("after link tian to hlink-tian, information of tian:\n");
	printf("%-10s %-10s\n", "i-node", "link count");
	printf("%-10ld %-10ld\n", (long)buf.st_ino, (long)buf.st_nlink);

	if (stat("hlink-tian", &buf) < 0) {
		printf("stat tian error: %s\n", strerror(errno));
		exit(1);
	}
	printf("information of hlink-tian:\n");
	printf("%-10s %-10s\n", "i-node", "link count");
	printf("%-10ld %-10ld\n", (long)buf.st_ino, (long)buf.st_nlink);

	exit(0);
}
