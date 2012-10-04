#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>

/* The user ID of a new file is set to the effective user ID of the process
 * POSIX.1 allows an implementation to choose one of the following options
 * to determine the group ID of a new file.
 * 1.The group ID of a new file can be the effective group ID of the process
 * 2.The group ID of a new file can be the group ID of the directory in
 *   which the file is being created.
 *
 * The rules for the ownership of a new directory are identical to the
 * rules in this section for the ownership of a new file.
 *
 * 在执行这个程序时,可以先用普通用户执行一次,再用根用户执行一次,看 stat()
 * 函数获取的 uid 和 gid 是否符合上面的规则.
 */
int main(int argc, char *argv[])
{
	struct stat buf;

	if (argc != 2) {
		fprintf(stderr, "Usage: %s <pathname>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	if (open(argv[1], O_RDWR | O_CREAT, 0644) < 0) {
		perror("open error");
		exit(EXIT_FAILURE);
	}

	if (stat(argv[1], &buf) < 0) {
		perror("stat error");
		exit(EXIT_FAILURE);
	}

	printf("%s: UID = %ld, GID = %ld\n", argv[1], 
			(long)buf.st_uid, (long)buf.st_gid);
	exit(EXIT_SUCCESS);
}
