#include "apue.h"

#define BUFFSIZE	4096

int main(void)
{
	char buf[BUFFSIZE];
	int n;

	while ((n = read(STDIN_FILENO, buf, BUFSIZ)) > 0)
		if (write(STDOUT_FILENO, buf, n) != n)
			err_sys("copyfile: error when writed to STDOUT_FILENO");
	if (n < 0)
		err_sys("copyfile: error when read from STDIN_FILENO");
	exit(0);
}
