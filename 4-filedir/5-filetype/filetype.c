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

		/* Historically, early versions of the UNIX System didn't provide
		 * the S_ISxxx macros. Instead, we had to logically AND the st_mode
		 * value with the mask S_IFMT and then compare the result with the
		 * constants whose names are S_IFxxx. Most systems define this mask
		 * and the related constants in the file <sys/stat.h>. If we examine
		 * this file, we'll find the S_ISDIR macro defined something like
		 *		#define S_ISDIR(mode)	(((mode) & S_IFMT) == S_IFDIR)
		 */
		switch (statbuf.st_mode & S_IFMT) {
			case S_IFBLK:
				ptr = "block special";
				break;
			case S_IFCHR:
				ptr = "charecter special";
				break;
			case S_IFDIR:
				ptr = "directory file";
				break;
			case S_IFIFO:
				ptr = "FIFO/pipe";
				break;
			case S_IFLNK:
				ptr = "symbolic link";
				break;
			case S_IFREG:
				ptr = "regular file";
				break;
			case S_IFSOCK:
				ptr = "socket";
				break;
			default:
				ptr = "** unknown mode **";
				break;
		}
		printf("%s\n", ptr);
	}
	exit(0);
}
