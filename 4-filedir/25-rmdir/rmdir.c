#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>

/* An empty directory is deleted with the rmdir() function. Recall that
 * an empty directory is one that contains entries only for dot and dot-dot
 * #include <unistd.h>
 * int rmdir(const char *pathname);
 * 		Returns: 0 if OK, -1 on error.
 * If the link count of the directory becomes 0 with this call, and if no
 * other process has the directory open, then the space occupied by the
 * directory is freed.
 *
 * 当指定的路径不是一个目录时,rmdir()函数会报错.
 * 当然,如果指定的目录不存在,rmdir()函数也会报错.
 */
int main(void)
{
	printf("当前目录下,有一个空目录test和一个非空目录test1\n");
	system("ls -l");
	if (rmdir("test") < 0) {
		printf("rmdir test error: %s\n", strerror(errno));
		return 1;
	}
	else {
		printf("删除空目录test, SUCCESS\n");
		system("ls -l");
	}

	if (rmdir("test1") < 0)
		printf("删除非空目录,rmdir test1 error: %s\n", strerror(errno));
	
	if (rmdir("tian") < 0)
		printf("删除普通文件,rmdir tian error: %s\n", strerror(errno));

	return 0;
}
