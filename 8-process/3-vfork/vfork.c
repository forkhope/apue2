#include <unistd.h>
#include <stdio.h>

int globval = 6;		/* external variable in initialized data */

/* The function vfork() has the same calling sequence and same return
 * values as fork(). But the semantics of the two functions differ.
 * The vfork() function is intended to create a new process when the
 * purpose of the new process is to exec a new program. The vfork() function
 * creates the new process, just like fork(), without copying the address
 * space of the parent into the child, as the child won't reference that
 * address psace; the child simply calls exec (or exit) right after the
 * vfork(). Instead, while the the child is running and until it calls
 * either exec or exit, the child runs in the address space of the parent.
 *
 * Another difference between the two functions is that vfork() guarantees
 * that the child runs first, until the child calls exec or exit. When the
 * child calls either of these functions, the parent resumes.
 * 这句话的意思是说,执行vfork()之后,子进程先被执行,此时父进程会陷入等待,直到
 * 子进程调用exec 或者exit 之前,父进程会一直都是等待状态,子进程调用exec 后,
 * 父进程和子进程在内核调度下分别得到执行机会.
 */
int main(void)
{
	int val;			/* automatic variable in the stack */
	pid_t pid;

	val = 88;
	printf("before vfork\n");		/* we don't flush stdout */
	if ((pid = vfork()) < 0) {
		printf("vfork error\n");
		return 1;
	}
	else if (pid == 0) { 	/* child */
		++globval;			/* modify parent's variables */
		++val;
		
		/* NOTE, we call _exit() instead of exit(). As we described in
		 * Section 7.3, _exit() does not perform any flushing of standard
		 * I/O buffers. If we call exit() instead, the results are
		 * indeterminate.
		 */
		_exit(0);			/* child terminates */
	}
	/* In here, we don't need to have the parent call sleep(), as we're
	 * guaranteed that it is put to sleep by the kernel until the child
	 * calls either exec or exit.
	 */

	/* parent continues here.
	 * Here, the incrementing of the variables done by the child change the
	 * values in the parent. Because the child runs in the address space of
	 * the parent, this doesn't surpise us. This behavior, however, differs
	 * from fork().
	 */
	printf("pid = %d, globval = %d, val = %d\n", getpid(), globval, val);
	return 0;
}
