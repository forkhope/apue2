#include <stdio.h>
#include <unistd.h>

/* In addition to having a process ID, each process also belongs to a
 * process group. A process group is a collection of one or more processes,
 * usually assoicated with the same job, that can receive signals from the
 * terminal. Each process group has a unique process group ID.
 * The function getpgrp returns the process group ID of the calling process.
 * #include <unistd.h>
 * pid_t getpgrp(void);
 * 		Returns: process group ID of calling process
 * 这个函数总是会执行成功,它没有指示出错的返回值.
 *
 * In older BSD-derived systems, the getpgrp() function took a pid argument
 * and returned the process group for that process. The Single UNIX
 * Specification defines the getpgid() function as an XSI extension that
 * mimics this behavior.
 * #include <unistd.h>
 * pid_t getpgid(pid_t pid);
 * 		Returns: process group ID if OK, -1 on error
 * If pid is 0, the process group ID of the calling process is returned.
 * Thus, "getpgid(0);" is equivalent to "getpgrp();"
 *
 * Each process group can have a process group leader. The leader is
 * identified by its process group ID being equal to its process ID.
 * 一个进程组的process group leader进程退出后,这个进程组有可能还存在.一个
 * 进程组的存在周期是从它被创建开始到组内最后一个进程离开这个组为止,和
 * process group leader进程是否退出没有关系.
 *
 * A process joins an existing process group or creates a new process group
 * by calling setpgid().
 * #include <unistd.h>
 * 		Returns: 0 if OK, -1 on error.
 * The function sets the process group ID to pgid in the process whose
 * process ID equals pid. If the two arguments are equal, the process
 * specified by pid becomes a process group leader. If pid is 0, the process
 * ID of the caller is used. Also, if pgid is 0, the process ID specified by
 * pid is used as the process group ID.
 *
 * A process can set the process group ID of only itself or any of its
 * children. Furthermore, it can't change the process group ID of one of
 * its children after that child has called one of the exec functions.
 */
int main(void)
{
	pid_t pgid, pid;

	pgid = getpgrp();	/* 获取当前进程的进程组ID */
	printf("The process group ID = %d\n", pgid);

	/* 使用 getpgid() 函数有个要注意的地方:
	 * 如果在编译程序时,指定了 -Wall 选项,则会提示:
	 * warning: implicit declaration of function ‘getpgid’
	 * 即使包含了 <sys/types.h>, <unistd.h> 头文件也是一样.
	 * 通过查找 man getpgid 的结果,得知,需要满足如下的特性测试宏:
	 * getpgid():
	 * 	Since glibc 2.12:
	 * 		_POSIX_C_SOURCE >= 200809L || _XOPEN_SOURCE >= 500 ||
	 *  	_XOPEN_SOURCE && _XOPEN_SOURCE_EXTENDED
	 *  Before glibc 2.12:
	 * 		_XOPEN_SOURCE >= 500 || _XOPEN_SOURCE && _XOPEN_SOURCE_EXTENDED
	 * 才能正确包含 getpgid() 函数的声明.
	 * 所以要满足上述的特性测试,才能正确声明 getpgid() 函数.
	 * 实际上,man getpgid手册中提到:如果要获取进程组ID,首选用getpgrp()
	 * 函数.也就是说,以后避免使用 getpgid() 函数.
	 *
	 * 为了消除上述的编译警告,可以在编译时指定如下的编译选项:
	 * 		-D_XOPEN_SOURCE=500
	 */
	if ((pgid = getpgid(0)) < 0)
		return 1;
	printf("getpgid(0) = %d\n", pgid);

	if ((pgid = getpgid(1)) < 0)
		return 1;
	printf("getpgid(1) = %d\n", pgid);

	pid = getpid();
	if (setpgid(pid, pid) < 0)	/* 设置当前进程的进程组ID为它的进程ID */
		return 1;
	printf("After setpgid(%d, %d), pgid = %d\n", pid, pid, getpgrp());

	return 0;
}
