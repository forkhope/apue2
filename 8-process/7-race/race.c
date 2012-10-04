#include <stdio.h>
#include <unistd.h>

static void characatime(char *str);

int main(void)
{
	pid_t pid;

	if ((pid = fork()) < 0) {
		printf("fork() error\n");
		return 1;
	}
	else if (pid == 0) {	/* child */
		characatime("output from child\n");
	}
	else {
		characatime("output from parent\n");
	}

	return 0;
}

static void characatime(char *str)
{
	setbuf(stdout, NULL);		/* stdout unbufferred */
	for (; *str != '\0'; ++str)
		putc(*str, stdout);
}
