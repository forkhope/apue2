#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <utime.h>
#include <stdlib.h>

/* 书中 4.19 小节提到: 使用 utime() 函数改变文件的访问时间和修改时间时,若
 * 第二个参数为NULL指针,则访问时间和修改时间都被设置为当前时间.但是这需要
 * 相应的权限: either the effective user ID of the process must equal the
 * owner ID of the file, or the process must have write permission for the
 * file. 当进程的 effective user ID 等于文件的 owner ID 时,这个操作就能调用
 * 成功,进程不需要对文件拥有写权限. 当进程的 effective user ID 不等于文件的
 * owner ID 时,进程需要对文件拥有组用户写权限或者其他用户写权限.如下:
 */
int main(void)
{
	printf("查看文件 tian 的权限,此时它没有写权限:\n");
	system("ls -l tian");

	printf("使用ps -f命令查看当前进程的ID,进程ID等于tian的用户ID:\n");
	system("ps -f");

	printf("对文件 tian 执行 utime(\"tian\", NULL),可以执行成功:\n");
	if (utime("tian", NULL) < 0)
		printf("utime tian error: %s\n", strerror(errno));
	else
		printf("utime tian NULL success\n");
	
	printf("查看文件 xia 的权限,它属于根用户所有,但是其他用户对它可写\n");
	system("ls -l xia");

	printf("对文件 xia 执行 utime(\"xia\", NULL),可以执行成功\n");
	if (utime("tian", NULL) < 0)
		printf("utime xia error: %s\n", strerror(errno));
	else
		printf("utime xia NULL success\n");

	return 0;
}
