#include <unistd.h>
#include <stdio.h>

/* Every process has a unique process ID, a non-negative integer.
 * In addition to the process ID, there are other identifiers for every
 * process. The following functions return these identifiers.
 * #include <unistd.h>
 * pid_t getpid(void);
 * 		Returns: process ID of calling process
 * pid_t getppid(void);
 *		Returns: parent process ID of calling process
 * uid_t getuid(void);
 * 		Returns: real user ID of calling process
 * uid_t geteuid(void);
 * 		Returns: effective user ID of calling process
 * gid_t getgid(void);
 * 		Returns: real group ID of calling process
 * gid_t getegid(void);
 * 		Returns: effective group ID of calling process
 *
 * Note that none of these functions has an error return.
 */
int main(void)
{
	printf("process ID: %d\n", getpid());
	printf("parent process ID: %d\n", getppid());
	printf("real user ID: %d\n", getuid());
	printf("effective user ID: %d\n", geteuid());
	printf("real group ID: %d\n", getgid());
	printf("effective group ID: %d\n", getegid());

	return 0;
}
