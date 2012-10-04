#include <stdio.h>		/* 声明perror()函数 */
#include <string.h>		/* 声明stderror()函数 */
#include <errno.h>		/* 声明 errno 变量 */
#include <stdlib.h>		/* 声明 exit() 函数 */

int main(int argc, char *argv[])
{
	/* char *strerror(int errnum);	// #include <string.h>
	 * This function maps errnum, which is typically the errno value, into
	 * an error message string and retuns a pointer to the string.
	 */
	fprintf(stderr, "EACCES: %s\n", strerror(EACCES));

	/* void perror(const char *msg);	// #include <stdio.h>
	 * The perror() function produces an error messsage on the standard
	 * error, based on the current value of errno, and retusn. It outputs
	 * the string pointed to by msg, followed by a colon and a space,
	 * followed by the error message corresponding to the value of errno,
	 * followed by a newline.
	 */
	errno = ENOENT;
	perror(argv[0]);

	/* strerror()函数根据传进来的整型值返回一个字符串指针,该指针指向预先写
	 * 好的错误信息,如果需要打印这个错误信息,需要另外调用打印函数来打印;
	 * perror()函数先打印参数所给的字符串,再根据errno当前的值打印出该错误码
	 * 对应的错误信息,两者以冒号和空格隔开; perror()包含了打印的功能.
	 */
	errno = EBADF;	/* 为 errno 赋值为 EBADF */
 	/* 打印所给参数字符串,再打印错误信息 */
	perror("errno等于EBADF时,错误信息为");
	exit(0);
}
