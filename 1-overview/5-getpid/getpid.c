#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void)
{
	/* A program is an executable file residing on disk in a directory.
	 * An executing instance of a program is called a process. The UNIX
	 * System guarantees that every process has a unique numeric identifier
	 * called the process ID. It is always a non-negative integer.
	 */
	printf("hello world from process ID %d\n", getpid());
	exit(0);
}
