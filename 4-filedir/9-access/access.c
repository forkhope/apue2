#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>		/* access() */
#include <fcntl.h>

/* access() - check real user's permissions for a file.
 * #include <unistd.h>
 * int access(const char *pathname, int mode);
 * 		Returns: 0 if ok, -1 on error.
 * 参数 mode 是以下四个常量的位或值 (bitwise OR), 例如, R_OK | W_OK.
 * 		R_OK:	test for read permission
 *		W_OK:	rest for write permission
 *		X_OK:	test for execute permission
 *		F_OK:	test for existence of file
 * access()函数基于进程的 real user ID 和 group IDs 来判断这些 ID 对文件
 * 的访问权限,该文件由函数的第一个参数指定. 书中前面的章节提到,内核会根据
 * 进程的 effective user ID 和 effective group ID 来判断进程对某个文件的
 * 访问权限.进程对文件的实际访问权限以 effective IDs 为准.如果进程运行时
 * 的 effective IDs 等于 real IDs,则 access() 查询到的访问权限就是实际的
 * 权限;但如果进程运行时的 effective IDs 不等于 real IDs (set-user-ID位
 * 可以做到这一点),则 access() 查询到的访问权限不代表进程对某个文件的实际
 * 访问权限. 书中给出这样一个例子(假设程序编译出来的文件名为access):
 * 1.在根用户下,使用 chown root access 目录将access文件的拥有者设为 root.
 * 2.执行 chmod u+s access命令,打开文件的 set-user-ID 位,这样执行 access
 *   这个文件,其 effective user ID 将会是 root.
 * 3.在普通用户下,执行 ./access /etc/shadow,会发现 access() 查询的权限为
 *   禁止,但是调用 read() 函数却能打开文件,没有报错.
 */
int main(int argc, char *argv[])
{
	if (argc != 2) {
		fprintf(stderr, "Usage: %s <pathname>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	/* 判断 real IDs 对文件是否拥有读权限 */
	if (access(argv[1], R_OK) < 0)
		printf("%s: access %s for R_OK error: %s\n", argv[0],
				argv[1], strerror(errno));
	else
		printf("%s: access %s for R_OK success\n", argv[0], argv[1]);

	/* 判断进程的 effective IDs 是否能用只读模式打开文件 */
	if (open(argv[1], O_RDONLY) < 0)
		printf("%s: open %s for O_RDONLY error: %s\n", argv[0],
				argv[1], strerror(errno));
	else
		printf("%s: open %s for O_RDONLY success\n", argv[0], argv[1]);
	
	/* 判断 read IDs 对文件是否拥有写权限 */
	if (access(argv[1], W_OK) < 0)
		printf("%s: access %s for W_OK error: %s\n", argv[0],
				argv[1], strerror(errno));
	else
		printf("%s: access %s for W_OK success\n", argv[0], argv[1]);

	/* 判断 read IDs 对文件是否拥有可执行权限 */
	if (access(argv[1], X_OK) < 0)
		printf("%s: access %s for X_OK error: %s\n", argv[0],
				argv[1], strerror(errno));
	else
		printf("%s: access %s for X_OK success\n", argv[0], argv[1]);

	/* 判断一个文件是否存在 */
	if (access(argv[1], F_OK) < 0)
		printf("%s: 文件 %s 不存在\n", argv[0], argv[1]);
	else
		printf("%s: 文件 %s 存在\n", argv[0], argv[1]);

	exit(EXIT_SUCCESS);
}
