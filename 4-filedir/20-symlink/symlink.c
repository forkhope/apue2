#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

/* A symbolik link is created with the symlink() function.
 * #include <unistd.h>
 * int symlink(const char *actualpath, const char *sympath);
 * 		Returns: 0 is OK, -1 on error.
 * 
 * A new directory entry, sympath, is created that points to actualpath. It
 * is not required that actualpath exist when the symbolic link is created.
 * Also, actualpath and sympath need not reside in the same file system.
 *
 * Symbolic links were introduced to get around the limitations of hard
 * links: 1. Hard links normally require that the link and the file reside
 * in the same file system. 2. Only the superuser can create a hard link
 * to a directory.
 * There are no file system limitations on a symbolic link and what it
 * points to, and anyone can create a symbolic link to a directory.
 *
 * When using functions that refer to a file by name, we always need to 
 * know whether the function follows a symbolic link. If the function
 * follows a symbolic link, a pathname argument to the function refers to
 * the file pointed to by the symbolic link. Othewise, a pathname argument
 * refers to the link itself, not the file pointed to by the link.
 * 书中4.16小节总结了部分函数是否跟随一个符号链接.其中,不会跟随符号链接的
 * 函数有: lchow(), lstat(), readlink(), remove(), rename(), unlink().
 */
int main(int argc, char *argv[])
{
	if (argc != 3) {
		printf("Usage: %s TARGET LINK_NAME\n", argv[0]);
		return 1;
	}

	if (symlink(argv[1], argv[2]) < 0)
		printf("symlink error: %s\n", strerror(errno));
	else
		printf("make symlink from %s to %s success\n",argv[0],argv[1]);

	return 0;
}
