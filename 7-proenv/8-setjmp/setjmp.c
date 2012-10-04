#include <setjmp.h>
#include <stdio.h>
#include <string.h>

/* 由于longjmp()函数的第一个参数和setjmp()函数的参数应该是同一个变量,
 * 所以下面的jmpbuffer要声明为全局变量,这样其他函数才能引用这个变量.
 */
jmp_buf jmpbuffer;

static void tianxia(void);
static void yitian(void);

/* setjmp() - save stack context for nonlocal goto.
 * longjmp()- nonlocal jump to a saved stack context.
 * #include <setjmp.h>
 * int setjmp<jmp_buf env);
 * 		Returns: 0 if called directly, nonzero if returning
 * 				 from a call to longjmp()
 * void longjmp(jmp_buf env, int val);
 * setjmp() and longjmp() are useful for dealing with errors and interrupts
 * encountered in a low-level subroutine of a program. setjmp() saves the
 * stack context/environment in env for later use by longjmp(). The stack
 * context will be invalidated if the function which called setjmp returns.
 *
 * longjmp() restores the environment saved by the last call of setjmp()
 * with the corresponding env argument. After longjmp() is completed,
 * program execution continues as if the corresponding call of setjmp() had
 * just returned the value val. longjmp() cannot cause 0 to be returned. If
 * longjmp() is invoked with a second argument of 0, 1 will be returned
 * instead.
 */
int main(void)
{
	char buf[BUFSIZ];
	int ret_from_setjmp;

	/* We call setjmp() function from the location that we want to return
	 * to, which in this example is in the main() function. In this case,
	 * setjmp() returns 0 because we called it directly. In the call to
	 * setjmp(), the argument env is of the special type jmp_buf. This data
	 * type is some form of array that is capable of holding all the
	 * information required to restore the status of the stack to the state
	 * when call longjmp(). Normally, the env variable is a global variable,
	 * since we'll need to reference it from another function.
	 *
	 * longjmp()函数会跳转会setjmp()函数所在的地方,且它的第二个参数正好是
	 * setjmp()函数的返回值.当setjmp()函数被直接调用,即不是通过longjmp()
	 * 跳转回来时,setjmp()函数会返回0.如果是通过longjmp()函数跳转回来的,
	 * setjmp()函数的返回值是longjmp()函数的第二个参数的值,longjmp()函数保
	 * 证它的第二个参数不会是0,如果指定longjmp()函数的第二个参数为0,该函数
	 * 不会报错,它会自动把这个值替换为1,即即使指定longjmp()函数的第二个参数
	 * 为0,setjmp()函数的返回值不会是0,而是1.
	 */
	if ((ret_from_setjmp = setjmp(jmpbuffer)) == 0)
		printf("Call setjmp() in main()\n");
	else if (ret_from_setjmp == 1)
		printf("Return from tianxia()\n");
	else if (ret_from_setjmp == 2)
		printf("Return from yitian()\n");
	else
		printf("Unknown error\n");

	while (fgets(buf, BUFSIZ, stdin) != NULL) {
		/* 下面的字符串"tianxia\n"中,'\n'是必不可少的,因为fgets()函数会连
		 * 用户输入的换行符一起读入,而用户从终端输入时,需要输入回车来结束
		 * 一次输入,也就是在这种情况下,fgets()函数返回的字符串中包含了换行
		 * 符,所以下面的"tianxia\n"要有'\n',这样strcmp()函数比较才能相等.
		 */
		if (strcmp(buf, "tianxia\n") == 0)
			tianxia();
		else if (strcmp(buf, "yitian\n") == 0)
			yitian();
		else
			printf("get a line: %s", buf);
	}

	return 0;
}

static void tianxia(void)
{
	printf("Enter tianxia()\n");
	// longjmp(jmpbuffer, 1);
	// 按照上面的描述,longjmp()函数的第二个参数是0时,这个是0会被替换为1 */
	longjmp(jmpbuffer, 0);
}

static void yitian(void)
{
	printf("Enter yitian()\n");
	longjmp(jmpbuffer, 2);
}
