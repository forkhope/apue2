#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void)
{
	/* getuid() returns the real user ID of the calling process.
	 * getgid() returns the real group ID of the calling process.
	 */
	printf("uid = %d, gid = %d\n", getuid(), getgid());
	exit(0);
}
