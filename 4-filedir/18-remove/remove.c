#include <stdio.h>
#include <string.h>
#include <errno.h>

/* We can also unlink a file or a directory with the remove() function. For
 * a file, remove() is identical to unlink(). For a directory, remove() is
 * identical to rmdir.
 * #include <stdio.h>
 * int remove(const char *pathname);
 * 	Returns: 0 if OK, -1 on error.
 *
 * ISO C specifies the remove() function to delete a file.
 */
int main(void)
{
	/* 执行该程序时, 文件tian 存在,看看它是否能执行成功 */
	if (remove("tian") < 0) 
		printf("remove tian error: %s\n", strerror(errno));
	else
		printf("remove tian success.....\n");

	/* 执行该程序时, 文件xia 不存在,看看它报的错是什么 */
	if (remove("xia") < 0)
		printf("remove xia error: %s\n", strerror(errno));

	return 0;
}
