#include <stdio.h>
#include <signal.h>

extern unsigned int sleep2(unsigned int signo);
static void sig_int(int signo);

/* use the sleep2() function */
int main(void)
{
	unsigned int unslept;

	if (signal(SIGINT, sig_int) == SIG_ERR)
		return 1;

	unslept = sleep2(5);
	printf("sleep2 returned: %u\n", unslept);

	return 0;
}

static void sig_int(int signo)
{
	int i, j;

	/* The integer k is declared volatile to prevent an optimizing compiler
	 * from discarding the loop.
	 */
	volatile int k;

	/* Tune these loops to run for more than 5 seconds
	 * on whatever system this test program is run.
	 */
	printf("sig_int() starting\n");
	for (k = 0, i = 0; i < 90000; ++i)
		for (j = 0; j < 20000; ++j)
			k += i * j;

	/* 上面的main()函数调用sleep2()函数,由于sleep2()函数本身的bug,当sig_int
	 * 函数上面的for循环执行超过5秒时,sleep2()函数调用longjmp()函数跳转到
	 * sleep2()函数,从而上面的main()函数从sleep2(5)语句往下执行,没有再跳回到
	 * sig_int()函数中;此时,下面的这个printf()语句没有得到执行.
	 * 即,由于sleep2()函数的bug,导致sig_int()函数没有执行完,且不再被执行.
	 */
	printf("sig_int() finished\n");
}
