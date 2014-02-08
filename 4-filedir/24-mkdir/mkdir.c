#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>

/* Directores are created with the mkdir () function.
 * #include <sys/stat.h>
 * int mkdir(const char *pathname, mode_t mode);
 * 		Returns: 0 if OK, -1 on error.
 * This function creates a new, empty directory. The entries for dot and
 * dot-dot are automatically created. The specified file access permissions,
 * mode, are modified by the file mode creation mask of the process.
 *
 * A common mistake is to specify the same mode as for a file: read and
 * write permissions only. But for a directory, we normally want at least
 * one of the execute bits enabled, to allow access to filenames within
 * the directory.
 *
 * 所创建目录的owner ID是进程的 effective user ID,其owner group ID可能是
 * 进程的 effective group ID,也可能是它的父目录的owner group ID.
 */
int main(int argc, char *argv[])
{
	mode_t mode;

	if (argc != 2) {
		printf("Usage: %s directory_name\n", argv[0]);
		return 1;
	}

	/* 下面的mode指定所有权限,但是所创建的目录并不拥有所有权限,因为这个
	 * 权限会被 umask() 掩码所修改. 另外,从执行结果看,mkdir()函数不能
     * 递归创建目录,例如传入参数"a/b/c"会报错:No such file or directory
     */
	mode = S_IRWXU | S_IRWXG | S_IRWXO;
	if (mkdir(argv[1], mode) < 0) {
		printf("mkdir error: %s\n", strerror(errno));
		return 1;
	}
	printf("由于被umask掩码所修改,最后的权限值被去掉了组和其他的写权限\n");
	system("ls -l");

	/* 如果所要创建的目录已经存在,则mkdir()函数会报错 */
	if (mkdir(argv[1], mode) < 0)
		printf("当所创建目录已存在时,mkdir error: %s\n", strerror(errno));

	return 0;
}
