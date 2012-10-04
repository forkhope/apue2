#include <errno.h>
#include <unistd.h>
#include <sys/wait.h>

/* The system() function, without signal handling.
 * 书中 8.13 小节对 system() 函数的返回值描述如下:
 * #include <stdlib.h>
 * int system(const char *cmdstring);
 * 		Returns: (see below)
 * If cmdstring is a null pointer, system() returns nonzero only if a
 * command processor is available. This feature determines whether the
 * system() function is supported on a given operating system. Under the
 * UNIX System, system() is always available.
 *
 * Because system() is implemented by calling fork(), exec(), and waitpid(),
 * there are three types of return values.
 * 1. If either the fork() fails or waitpid() returns an error other than
 * EINTR, system() returns -1 with errno set to indicate the error.
 * 2. If the exec() fails, implying that the shell can't be executed, the
 * return value is as if the shell had executed exit(127).
 * 3. Otherwise, all three functions--fork(), exec(), and waitpid()--succeed
 * and the return value from system() is the termination status of the shell
 * in the format specified for waitpid().
 */
int System(const char *cmdstring)
{
	pid_t pid;
	int status;

	if (cmdstring == NULL)
		return 1;		/* always a command processor with UNIX */

	if ((pid = fork()) < 0)
		status = -1;	/* probably out of processor */
	else if (pid == 0) {	/* child */
		/* The shell's -c option tells it to take the next command-line
		 * argument--cmdstring, in this case--as its command input instead
		 * of reading from standard input or from a given file. The shell
		 * parses this null-terminated C string and breaks it up into
		 * separate command-line arguments for the command.
		 */
		execl("/bin/sh", "sh", "-c", cmdstring, (char *)0);

		/* Note that we call _exit() instead of exit(). We do this to
		 * prevent any standard I/O buffers, which would have been copied
		 * from the parent to the child across the fork(), from being
		 * flushed in the child.
		 * 即,使用 _exit() 来避免在子进程中刷新从父进程复制过来的I/O缓冲区
		 * 信息,这些信息是父进程本身的信息,不是此时子进程的所要打印信息,如
		 * 果子进程打印出这些信息,反而会带来一些误解.
		 */
		_exit(127);		/* execl() error */
	}
	else {	/* parent */
		while (waitpid(pid, &status, 0) < 0)
			if (errno != EINTR) {
				status = -1;	/* error other than EINTR fro waitpid() */
				break;
			}
	}

	return status;
}
