#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>

/* The chown() functions allow us to change the user ID of a file and a
 * group ID of a file.			#include <unistd.h>
 *		int chown(const char *pathname, uid_t owner, gid_t group);
 *		int fchown(int filedes, uid_t owner, gid_t group);
 *		int lchown(const char *pathname, uid_t owner, gid_t group);
 *								All three return: 0 if OK, -1 on error.
 *
 * If either of the arguments owner or group is -1, the corresponding ID
 * is left unchanged.
 * 在Linux系统中,只有超级用户(superuser)能调用chown()改变一个文件的user ID
 * 一个不拥有超级用户权限的进程调用 chown() 函数改变文件的user ID会报错,
 * 即使该进程是那个文件的拥有者也是一样.详细的描述如下:
 * 1. Only a superuser process can change the user ID of the file.
 * 2. A nonsuperuser process can change the group ID of the file if the
 *    process owns the file (the effective user ID equals the user ID of
 *    the file), owner is specified as -1 or equals the user ID of the
 *	  file, and group equals either the effective group ID of the process
 *	  or one of the process's supplementary group IDs.
 * This means you can't change the user ID of other user's files. You can
 * change the group ID of files that you own, but only to groups that you
 * belong to.
 * 另外,书中提到下面一点:
 * If these functions are called by a process other than a superuser
 * process, on successful return, both the set-user-ID and the set-group-ID
 * bits are cleared.
 * 在Linux系统上测试,发现实际情况和书中描述有所偏差.如果一个文件设置了
 * set-user-ID 和 set-group-ID,则当文件的S_IXUSR为0,即文件的拥有者没有可执
 * 行权限时,即使 chown() 函数成功返回, set-user-ID 也不会被清空,当S_IXUSR
 * 为 1 时, chown() 函数返回后, set-user-ID 位才会被清空. set-group-ID类似.
 * 在Linux系统的man手册中,对 chown() 函数有如下的说明:
 *		When the owner or group of an executable file are changed by an
 * 		unprivileged user the S_ISUID and S_ISGID mode bits are cleared.
 *		POSIX does not specify whether this also should happed when root
 *      the chown();
 * 这段说明可以作为上面测试结果的解释.
 */
int main(void)
{
	/* 下面调用 chown() 函数试图将 foo 文件的 user ID 和 group ID 改为1001
	 * 函数将会报错,errno被设为EPERM,对应的描述为"Operation not permitted"
	 */
	if (chown("foo", 1001, 1001) < 0)
		printf("chown foo error: %s\n", strerror(errno));

	/* 按照上面的描述, 不拥有超级用户的进程可以改变它所拥有文件的 group ID,
     * 但只限于修改为该进程所属的 group IDs. 下面试图将foo文件的 group ID
	 * 修改为1001,由于进程不属于该 group ID,因此同样会报错.
	 */
	if (chown("foo", -1, 1001) < 0)
		printf("chown foo error: %s\n", strerror(errno));

	printf("=====================================\n");
	printf("进程的 effective group ID 等于 %d\n", getgid());
	printf("=====================================\n");
	system("sudo chown :root foo; ls -l foo");

	/* 上面 system() 函数执行 shell 命令,将 foo 文件的组用户修改为根用户.
	 * 由于进程的 effective group ID 是1000,所以下面的语句能够调用成功.
	 */
	if (chown("foo", 1000, 1000) < 0)
		printf("chown foo error: %s\n", strerror(errno));
	system("ls -l foo");

	printf("-------------------------------------\n");
	system("sudo chown :root foo; ls -l foo");
	if (chown("foo", -1, 1000) < 0) // 将第二个参数设为 -1,也能调用成功
		printf("chown foo error: %s\n", strerror(errno));
	system("ls -l foo");

	exit(0);
}
