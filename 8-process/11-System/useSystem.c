#include <stdio.h>

extern int System(const char *cmdstring);
void pr_exit(int status);

/* 书中 8.13 小节提到了 system() 函数的安全隐患.描述如下:
 * What happends if we call system() from a set-user-ID program? Doing so
 * is a security hole and should never be done. 因为此时, system() 函数所
 * 创建的子进程的effective user ID和父进程的effective user ID是一样的.
 *
 * If it is running with special permissions--eithere set-user-ID or set-
 * group-ID--and wants to spawn another process, a process should use fork()
 * and exec() directly, being certain to change back to normal permissions
 * after the fork(), before calling exec(). The system() function should
 * never be used from a set-user-ID or a set-groupd-ID program.
 * 注意,这里并不是说,直接调用fork()和exec()函数,这两个函数会自动把进程的权限
 * 还原回(change back)普通权限,而是说程序在调用fork()之后,由程序本身来把权限
 * 还原回普通权限,再调用exec()函数执行另外的程序. fork()函数创建的子进程会继
 * 承父进程的real user ID和effective user ID.fork()函数并不会自动把子进程的
 * 权限还原回普通权限,这段话的意思是说,由程序本身来做这个切换.
 */
int main(void)
{
	int status;

	if ((status = System("date")) < 0)
		return 1;
	pr_exit(status);

	if ((status = System("nosuchcmd")) < 0)
		return 1;
	pr_exit(status);

	if ((status = System("who; exit 44")) < 0)
		return 1;
	pr_exit(status);

	return 0;
}
