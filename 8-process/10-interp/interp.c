#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

/* All contemporary UNIX systems support interpreter file. These files are
 * text files that begin with a line of the form
 * 		#! pathname [optional-argument]
 * The space between the exclamation point and the pathname is optional.
 * The pathname is normally an absolute pathname, since no special
 * operations are performed on it (i.e., PATH is not used).
 * The actual file that gets executed by the kernel is not the interpreter
 * file, but the file specified by the pathname on the first line of the
 * interpreter file. Be sure to differentiate between the interpreter file
 * --a text file that begins with #! --and the interpreter, which is
 * specified by the pathname on the first line of the interpreter file.
 */
int main(void)
{
	pid_t pid;

	if ((pid = fork()) < 0)
		return 1;
	else if (pid == 0) {	/* child */
		/* 新建 interpreter 文件时,没有给该文件添加可执行权限,导致运行时,
		 * execl()函数报错: execl error: Permission denied
		 * 使用 chmod +x interpreter 命令给 interpreter 文件添加可执行权限
		 * 后, execl()函数才能成功解释(interpreter)该文件.
		 *
		 * 假设有一个 /home/sar/bin/testinterp 文件,其内容为:
		 * 		#!/home/sar/bin/echoarg foo
		 * 则对此,有下面的描述:
		 * Note that when the kernel execs the interpreter (/home/sar/bin/
		 * echoarg), argv[0] is the pathname of the interpter, 即/home/sar/
		 * bin/echoarg.这里注意 interpreter 是指interpreter file中所指定要
		 * 执行的程序.然后, argv[1] is the optional argument from the
		 * interpreter file, 即上面的foo. 注意,这里如果写为下面的形式:
		 * 		#!/home/sar/bin/echoarg foo bar
		 * 则argv[1]是foo bar,而不是说argv[1]是foo, argv[2]是bar. 这里有点
		 * 迷惑,因为一般用空格隔开的两个字符串会被看作两个参数,但实际测试
		 * 得到的结果显示,这里即使用空格隔开,interpreter file中参数还是会被
		 * 当作一个整体赋给argv[1].
		 * 最后, the remaining arguments are the pathname (/home/sar/bin/
		 * testinterp) and the second and third arguments from the call to
		 * execl(). Both argv[1] and argv[2] from the call to execl() have
		 * been shifted right two positions. Note that the kernel takes the
		 * pathname from the execl() call instead of the first argument
		 * (testinterp), on the assumption that the pathname might contain
		 * more information than the first argument. 注意,这里说的是,剩下的
		 * 参数是传给execl()函数的路径名(pathname),argv[1],和argv[2];argv[0]
		 * 没有被使用.也就是下面这个语句中的argv[0]--"interpreter file"会被
		 * execl()函数的第一个参数"interpreter"取代.这个程序的执行结果中,
		 * argv[2]的值是"interpreter",而不是"interpreter file".要注意这一点
		 */
		if (execl("interpreter","interpreter file","ARG1",(char *)0)<0) {
			printf("execl error: %m\n");
			return 1;
		}
	}

	if (waitpid(pid, NULL, 0) < 0)
		return 1;

	return 0;
}
