#include <stdio.h>

/* #include <stdio.h>
 * int fileno(FILE *stream);
 * The function fileno() examines the argument stream and returns its
 * integer descripter.
 */
int main(void)
{
	int fd_stdin, fd_stdout, fd_stderr;

	fd_stdin = fileno(stdin);
	fd_stdout = fileno(stdout);
	fd_stderr = fileno(stderr);

	printf("fd_stdin = %d\n", fd_stdin);
	printf("fd_stdout = %d\n", fd_stdout);
	printf("fd_stderr = %d\n", fd_stderr);

	return 0;
}
