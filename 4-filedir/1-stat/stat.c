#include <sys/stat.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

/* int stat(const char *path, struct stat *buf);
 * stat() function returns information about a file. No permissions are
 * required on the file itself, but execute (search) permission is required
 * on all of the directories in path that lead to the file.
 *
 * 如果 stat() 函数的第一个参数path指向一个链接文件,则stat()函数真正返回的
 * 是该链接文件指向的文件,即 stat() 函数无法获取到一个链接文件本身的信息;
 * 如果要获取一个链接文件本身的信息,可以使用 lstat() 函数.
 *
 * 结构体 struct stat 一般有 13 个成员, 它们都已 st_t 开头, 如下:
 * st_dev,		st_ino,			st_mode,		st_nlink,
 * st_uid,		st_gid,			st_rdev,		st_size,
 * st_blksize,	st_blocks,		st_atime,		st_mtime,	st_ctime
 */
int main(int argc, char *argv[])
{
	struct stat sb;

	if (argc != 2) {
		fprintf(stderr, "Usage: %s <pathname>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	if (stat(argv[1], &sb) == -1) {
		perror("stat");
		exit(EXIT_FAILURE);
	}

	/* 判断文件类型 */
	printf("File type:                ");
	switch (sb.st_mode & S_IFMT) {
		case S_IFBLK:
			printf("block device\n");
			break;
		case S_IFCHR:
			printf("character device\n");
			break;
		case S_IFDIR:
			printf("directory\n");
			break;
		case S_IFIFO:
			printf("FIFO/pipe\n");
			break;
		case S_IFLNK:
			printf("symbolic link\n");
			break;
		case S_IFREG:
			printf("regular file\n");
			break;
		case S_IFSOCK:
			printf("socket\n");
			break;
		default:
			printf("unknown?\n");
			break;
	}

	printf("I-node number:            %ld\n", (long)sb.st_ino);

	printf("Mode:                     %lo (octal)\n",
			(unsigned long)sb.st_mode);

	printf("Link count:               %ld\n", (long)sb.st_nlink);

	printf("Ownership:                UID=%ld   GID=%ld\n",
			(long)sb.st_uid, (long)sb.st_gid);

	printf("Preferred I/O block size: %ld bytes\n",
			(long)sb.st_blksize);

	printf("File size:                %lld bytes\n",
			(long long)sb.st_size);

	printf("Blocks allocated:         %lld\n",
			(long long)sb.st_blocks);

	printf("Last status change:       %s", ctime(&sb.st_ctime));
	printf("Last file access:         %s", ctime(&sb.st_atime));
	printf("Last file modification:   %s", ctime(&sb.st_mtime));

	exit(EXIT_SUCCESS);
}
