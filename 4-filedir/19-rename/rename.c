#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>

/* A file or a directory is renamed with the rename() function.
 * #include <stdio.h>
 * int rename(const char *oldname, const char *newname);
 * 	Returns: 0 if OK, -1 on error.
 *
 * rename()函数在使用的时候有一些要注意的地方,具体参见书中第4.15小节
 */
int main(void)
{
	system("ls -l");
	printf("将文件tian 重命名为 xia\n");
	if (rename("tian", "xia") < 0)
		printf("rename tian to xia error: %s\n", strerror(errno));

	system("ls -l");
	return 0;
}
