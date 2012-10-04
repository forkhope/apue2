#include <stdio.h>

/* When a program is executed, the process that does the exec() can pass
 * command-line arguments to the new program.
 */
int main(int argc, char *argv[])
{
	int i;

	for (i = 0; i < argc; ++i)	/* echo all command-line args */
		printf("argv[%d]: %s\n", i, argv[i]);

	printf("Again.....................\n");

	/* We are guaranteed by by both ISO C and POSIX.1 that argv[argc] is
	 * a null pointer. This lets us alternatively code the
	 * argument-processing loop as
	 */
	for (i = 0; argv[i] != NULL; ++i)
		printf("argv[%d]: %s\n", i, argv[i]);

	return 0;
}
