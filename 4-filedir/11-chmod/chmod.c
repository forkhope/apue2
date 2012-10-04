#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>

/* These two functions allow us to change the file access permissions for
 * an existing file.
 * #include <sys/stat.h>
 * int chmod(const char *pathname, mode_t mode);
 * int fchmod(int filedes, mode_t mode);
 * 		Both return: 0 if OK, -1 on error.
 * The chmod() function operates on the specified file, whereas the
 * fchmod() function operates on a file that has already been opened.
 *
 * To change the permission bits of a file, the effective user ID of the
 * process must be equal to the owner ID of the file, or the process
 * must have superuser permissions.
 *
 * 书中提到可以改变的权限位有 15 个,如下:
 * 		S_ISUID,		set-user-ID on execution
 *		S_ISGID,		set-group-ID on execution
 *		S_ISVTX,		saved-text (sticky bit)
 *		S_IRWXU,		read, write, and execute by user (owner)
 *		S_IRUSR,		read by user (owner)
 *		S_IWUSR,		write by user (owner)
 *		S_IXUSR,		execute by user (owner)
 *		S_IRWXG,		read, write and execute by group
 *		S_IRGRP,		read by group
 *		S_IWGRP,		write by group
 *		S_IXGRP,		execute by group
 *		S_IRWXO,		read, write, and execute by other
 *		S_IROTH,		read by other
 *		S_IWOTH,		write by other
 * 		S_IXOTH,		execute by other
 */
int main(void)
{
	struct stat statbuf;
	int fd;

	/* For the file foo, we set the permissions relative to their current
	 * state. To do this, we first call stat() to obtain the current
	 * permissions and then modify them.
	 */
	if (stat("foo", &statbuf) < 0) {
		printf("stat foo error: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	if (chmod("foo", (statbuf.st_mode & ~S_IXGRP) | S_ISGID) < 0) {
		printf("chmod foo error: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	if ((fd = open("bar", O_RDWR)) < 0) {
		printf("open bar error: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	/* We have set the permissions of the file bar to an absolute value,
	 * regardless of the current permission bits.
	 */
	if (fchmod(fd, S_IRUSR | S_IWUSR | S_IWGRP | S_IWOTH) < 0) {
		printf("chmod bar error: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	exit(EXIT_SUCCESS);
}
