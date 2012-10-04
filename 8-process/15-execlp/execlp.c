#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

/* In the program shown in Figure 8.20, we call execl, specifying the
 * pathname of the interpreter file. If we called execlp instead, specifying
 * a filename of testinterp, and if the directory /home/sar/bin was a path
 * prefix, what would be printed as argv[2] when the program is run?
 *
 * 对于这个问题,书中给出的答案是:
 * The same value (/home/sar/bin/testinterp) is printed for argv[2. The
 * reason is that execlp ends up calling execve with the same pathnames as
 * when we call execl directly. Recall Figure 8.15.
 *
 * 这道练习题应该是想说明 execlp() 函数会先做一些参数的组合工作,最终会调用
 * execve() 函数来执行一个可执行程序,需要注意这两个函数内在的调用关系.
 *
 * 测试的结果和这个答案相符.不过有个要注意的地方,描述如下:
 */
int main(void)
{
	pid_t pid;

	if ((pid = fork()) < 0)
		return 1;
	else if (pid == 0) { /* child */
		/* 执行这个程序需要把当前目录添加到PATH环境变量中去,执行时,可以在
		 * shell中执行如下命令:
		 * $export PATH=$PATH:/home/.../15-execlp
		 * (中间省略了部分路径)
		 * 注意,这里一定要写绝对路径,不能写为 export PATH=$PATH:
		 * 否则,执行的结果将和书中答案描述的不符.
		 * 这是因为写为 $PATH: 的形式时,虽然会在当前目录下查找文件,但是它
		 * 不会自动将当前目录的路径添加到文件名前面.所以此时打印出来的文件
		 * 名不包含当前目录的路径.即:
		 * 1.export PATH=$PATH:/home/.../15-execlp
		 * 则打印出来的argv[2]是 /home/.../15-execlp/interpreter
		 * 2.export PATH=$PATH:
		 * 则打印出来的argv[2]是 interpreter
		 * 3.export PATH=$PATH:.
		 * 则打印出来的argv[2]是 ./interpreter
		 */
		if (execlp("interpreter","interp file","ARG1",(char *)0) < 0) {
			printf("execlp error: %m\n");
			return 1;
		}
	}
	
	if (waitpid(pid, NULL, 0) < 0)
		return 1;

	return 0;
}
