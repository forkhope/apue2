#include "apue.h"
#include <sys/stat.h>

/* Print type of file for each command-line argument.
 * 
 * UNIX系统共有 7 种文件类型,如下:
 * The type of a file is encoded in the st_mode member of the stat
 * structure. We can determine the file type with the macros:
 * 		S_ISREG():	regular file
 *		S_ISDIR():	directory file
 *		S_ISCHR():	charecter special file
 *		S_ISBLK():	block special file
 *		S_ISFIFO():	pipe or FIFO
 *		S_ISLNK():	symbolic link
 * 		S_ISSOCK():	socket
 * The argument to each of these macros is the st_mode member from
 * the stat structure.
 */
int main(int argc, char *argv[])
{
	int i;
	struct stat statbuf;
	char *ptr;

	for (i = 1; i < argc; ++i) {
		printf("%s: ", argv[i]);
		if (lstat(argv[i], &statbuf) < 0) {
			err_ret("%s: lstat error", argv[0]);
			continue;
		}

		if (S_ISREG(statbuf.st_mode))
			ptr = "regular";
		else if (S_ISDIR(statbuf.st_mode))
			ptr = "directory";
		else if (S_ISLNK(statbuf.st_mode))
			ptr = "symbolic link";
		else if (S_ISBLK(statbuf.st_mode))
			ptr = "block special";
		else if (S_ISCHR(statbuf.st_mode))
			ptr = "character special";
		else if (S_ISSOCK(statbuf.st_mode))
			ptr = "socket";
		else if (S_ISFIFO(statbuf.st_mode))
			ptr = "FIFO/pipe";
		else
			ptr = "** unknown mode **";
		printf("%s\n", ptr);
	}
	exit(0);
}
