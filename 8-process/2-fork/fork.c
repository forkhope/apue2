#include <unistd.h>
#include <stdio.h>

int globval = 6;		/* external variable in initialized data */
char buf[] = "a write to stdout\n";

/* An existing process can create a new one by calling the fork() function.
 * #include <unistd.h>
 * pid_t fork(void);
 * 		Returns: 0 if child, process ID of child in parent, -1 on error.
 * The new process created by fork() is called the child process. This
 * function is called once but returns twice. The only difference in the
 * return is that the return value in the child is 0, whereas the return
 * value in the parent is the process ID of the new child.
 *
 * The child is a copy of the parent. For example, the child gets a copy of
 * the parent's data space, heap, and stack. Note that this is a copy for
 * the child; the parent and the child does not share these portions of
 * memory. The parent and the child share the text segment.
 *
 * In general, we never know whether the child starts executing before the
 * parent or vice versa.
 *
 * In this example routine, when we run the program interactively, we get
 * only a single copy of the printf() line, because the standard output
 * buffer is flushed by the newline. In this second case, the printf()
 * before the fork() is called once, but the line remains in the buffer
 * when fork() is called. This buffer is then copied into the child when
 * the parent's data space is copied to the child. Both the parent and the
 * child now have a standard I/O buffer with this line in it. The second
 * printf(), right before the exit(), just appends its data to the existing
 * buffer. When each process terminates, its copy of the buffer is finally
 * flushed.
 *
 * One characteristic of fork() is that all file descriptors that are open
 * in the parent are duplicated in the child. We say "duplicated" because
 * it's as if the dup() function had been called for each descriptor. The
 * parent and the child share a file table entry for every open descriptor.
 */
int main(void)
{
	int autoval;		/* automatic variable on the stack */
	pid_t pid;

	autoval = 88;
	if (write(STDOUT_FILENO, buf, sizeof(buf)-1) != sizeof(buf)-1)
		printf("write %s to stdout error\n", buf);
	printf("before fork\n");	/* we don't flush stdout */

	if ((pid = fork()) < 0) {
		printf("fork error\n");
		return 1;
	}
	else if (pid == 0)	{	/* child */
		++globval;			/* modity variables */
		++autoval;
	}
	else					/* parent */
		sleep(2);

	printf("pid = %d, global = %d, autoval = %d\n", 
			getpid(), globval, autoval);
	return 0;
}
