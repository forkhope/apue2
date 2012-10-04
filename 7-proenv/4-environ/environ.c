#include <stdio.h>
#include <unistd.h>

extern char **environ;

/* man手册对 environ 的描述如下:
 * The variable environ points to an array of pointers to strings called
 * the "environment". The last pointer in this array has the value NULL.
 * This variable must be declared in the user program.
 *
 * APUE2书中7.5 Environment List小节对 environ 的描述如下:
 * Each program is also passed an environment list. Like the argument list,
 * the environment list is an array of character pointers, with each
 * pointer containing the address of a null-terminated C string. The address
 * of the array of pointers is contained in the global variable environ:
 * 		extern char **environ;
 * By convention, the environment consists of "name=value" string. Most
 * predifined names are entirely uppercase, but this is only a convention.
 */
int main(void)
{
	int i;

	/* The last pointer in this array has the value NULL. */
	for (i = 0; environ[i] != NULL; ++i)
		printf("%s\n", environ[i]);

	return 0;
}
