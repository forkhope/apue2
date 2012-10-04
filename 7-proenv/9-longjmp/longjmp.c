#include <setjmp.h>
#include <stdio.h>

static void f1(int, int, int, int);
static void f2(void);

static int globval;
jmp_buf jmpbuffer;

/* We've seen what the stack looks like after calling longjmp(). The next
 * question is, "what are the states of the automatic variables and
 * register variables in the main() function?". When main() is returned to
 * by the longjmp(), do these variables have values corresponding to when
 * the setjmp() was previously called (i.e., are their values rolled back),
 * or are their values left alone so that their values are whatever they
 * were when do_line was called? Unfortunately, the answer is "it depends".
 *
 * 注意对比一下不同的编译选项(指是否指定-O选项)对执行结果的影响.
 */
int main(void)
{
	int autoval;
	register int regival;
	volatile int volaval;
	static int statval;

	autoval = 2;
	regival = 3;
	volaval = 4;
	statval = 5;
	globval = 6;

	if (setjmp(jmpbuffer) != 0) {
		printf("return from longjmp:\n");
		printf("autoval = %d, regival = %d, volaval = %d, "
			   "statval = %d, globval = %d\n", autoval, regival,
			   volaval, statval, globval);
		return 0;
	}

	/* Change variables after setjmp(), but before longjmp(). */
	autoval = 92;
	regival = 93;
	volaval = 94;
	statval = 95;
	globval = 96;

	f1(autoval, regival, statval, globval);
	return 0;	/* 这一条语句不会被执行到 */
}

static void f1(int autoval, int regival, int statval, int volaval)
{
	printf("Enter f1()\n");
	printf("autoval = %d, regival = %d, volaval = %d, "
		   "statval = %d, globval = %d\n", autoval, regival,
		   volaval, statval, globval);
	f2();
}

static void f2(void)
{
	longjmp(jmpbuffer, 1);
}
