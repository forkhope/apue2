#include "apue.h"
#include <sys/stat.h>

/* The st_size member of the stat structure contains the size of the file
 * in bytes. This field is meaningful only for regular files, directories,
 * and symbolic links.
 * 
 * 1. For a regular file, a file size of 0 is allowed. We'll get an
 * end-of-file indication on the first read of the file.
 * 2. For a directory, the file size is usually a multiple of a number,
 * such as 16 or 512. (这个固定的长度可能是对应某个结构体)
 * 3. For a symbolic link, the file size is the number of bytes in the
 * filename.(Note that symbolic links do not contain the normal C null byte
 * at the end of the name, as the length is always specified by st_size
 */
int main(int argc, char *argv[])
{
	struct stat buf;

	if (argc != 2)
		err_quit("Usage: %s <pathname>", argv[0]);

	/* 使用 lstat() 函数以便获取到 symbolic link 文件本身的信息 */
	if (lstat(argv[1], &buf) < 0)
		err_sys("%s: stat %s error", argv[0], argv[1]);

	printf("The size of %s is: %lld\n", argv[1], (long long)buf.st_size);

	exit(0);
}
