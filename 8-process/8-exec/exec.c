#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

/* env_list[] 需要用 NULL 结尾 */
char *env_list[] = {"USER=unknown", "PATH=/tmp", NULL};

/* When a process calls one of the exec functions, that process is
 * completely replaced by the new program, and the new program starts
 * executing at its main() function. The process ID does not change across
 * an exec, because a new process is not created; exec merely replaces the
 * current process--its text, data, heap, and stack segments--with a
 * brand new program from disk.
 * #include <unistd.h>
 * int execl(const char *pathname, const char *arg0, ... // (char *)0);
 * int execv(const char *pathname, char *const argv[]);
 * int execle(const char *pathname, const char *arg0, ...
 *                 // (char *)0, char *const envp[]);
 * int execve(const char *pathname, char *const argv[], char *const envp[]);
 * int execlp(const char *filename, const char *arg0, ... // (char *)0);
 * int execvp(const char *filename, char *const argv[]);
 *         All six return: -1 on error, no return on success
 *
 * 下面的程序描述的是 execl 类型的函数, exev 类型的函数和 execl 类似.
 */
int main(void)
{
    pid_t pid;
    char current_dir[BUFSIZ];

    if ((pid = fork()) < 0) {
        printf("fork error\n");
        return 1;
    }
    else if (pid == 0) {     /* child */
        /* execl()函数的第一个参数是一个路径名.书中1.4章节对路径名pathname
         * 的概念做过描述:A sequence of one or more filenames, separated by
         * slashed and optionally starting with a slash, forms a pathname.
         * A pathname that begins with a slash is called an absolute 
         * pathname; otherwise, it's called a relative pathname.Relative 
         * pathnames refer to files relative to the current directory.
         * 即,任何一个不是以斜线开头的路径名都是相对路径名.相对路径名并不一
         * 定是以 ./ 或者 ../ 开头.例如 echoall 也是一个路径名,由于相对路径
         * 是相对于当前目录来说,所以程序会在当前目录下查找 echoall 文件.
         *
         * 由于shell的PATH环境变量中一般不包含当前目录,假如在当前目录下有个
         * echoall 可执行文件,想要在shell下执行这个文件,需要输入: ./echoall
         * 直接输入echoall是不行的.而 execl() 函数也是会执行一个可执行文件,
         * 我一开始有点误解,以为写为 execl("echoall",..)是不行的,执行会报错
         * 但实际上没有,这种写法是正确的. "echoall" 就是一个有效的路径名,
         * execl()函数会在当前目录下查找名为 echoall 的文件,就像调用open()
         * 函数时一样,语句open("a.txt",...)会在当前目录下打开a.txt文件,如果
         * 存在该文件,则打开成功,否则打开失败. open()函数也是用路径名作为第
         * 一个参数,和execl()一样,不至于说需要写为open("./a.txt",...)的形式
         *
         * 由于shell下,执行当前目录下的文件一般要输入 ./xxxx,导致误解,以为
         * 引用当前目录下的可执行文件时,路径名一定要写为 ./xxxx 的形式,直接
         * 使用 xxxx 作为参数就不行.现在来看,不是这样的.
         *
         * 书中1.6小节实现的简易版shell,可以用来解释为什么执行当前目录下的
         * 文件一般要输入 ./xxxx, 这里假设PATH环境变量中不包含当前目录.
         * 书中1.6小节使用execlp()函数来执行用户输入的命令,execlp()函数的第
         * 一个参数是filename,是一个文件名,如果该文件名不带有任何的斜线/,则
         * execlp()函数会在这个文件名的前面添加PATH中包含的路径前缀来得到一
         * 个路径名,并去查找这个路径名指定的文件.而如果该文件名中带了斜线,
         * 这个文件名就被看作路径名,execlp()会直接去查找这个路径名指定的文件
         * 所以,当PATH=/bin:/usr/bin时,在shell下面输入xxxx, shell所调用的
         * execlp()函数会分别去查找/bin/xxxx 和 /usr/bin/xxxx 文件,且不会在
         * 当前目录下查找xxxx文件.若在shell下输入./xxxx,则shell只在当前目录
         * 下查找xxxx.这就是 ./xxxx 和 xxxx 在shell下的区别.
         * 当然,linux下的shell不一定是调用execlp()函数来执行用户输入的命令,
         * 但是这个解释对书中1.6小节所实现的简易版shell是能解释得通的.
         *
         * C语言中有这样的惯例,传递给main()函数的参数中,argv[0]是所执行程序
         * 的名字,但这仅仅是惯例.实际上,argv[0]的值可以是任意字符串. We can
         * set argv[0] to any string we like.
         */
        if (execl("echoall", "EXECL ARG1", "arg2", (char *)0) < 0) {
            printf("execl echoall error: %m\n");
            return 1;
        }
    }

    if (waitpid(pid, NULL, 0) < 0) {
        printf("waitpid error\n");
        return 1;
    }

    if ((pid = fork()) < 0) {
        printf("fork error\n");
        return 0;
    }
    else if (pid == 0) {     /* child */
        /* 获取当前目录的路径 */
        if (getcwd(current_dir, BUFSIZ) == NULL) {
            printf("getcwd error: %m\n");
            return 1;
        }

        /* 添加所要执行的当前目录下的文件名.注意, getcwd() 返回的当前目录
         * 路径名中,末尾并没有包含斜线/,所以下面要在echoall前面加上斜线/
         * 否则添加后的路径名是错的.
         */
        strcat(current_dir, "/echoall");

        /* execle()函数会传递一个新的环境变量,并用这个新的环境变量取代原先
         * 的环境变量.此时,原先的环境变量已经不再存在,相当于是先全部清空原
         * 先的环境变量,再设置新的环境变量.而不是添加新的环境变量到老的环境
         * 变量中. execve() 函数的情况和 execle() 函数的情况类似.
         * 其他四个exec函数使用environ变量来将原先的环境变量传递到新程序中
         */
        if (execle(current_dir, "echoall", "myarg1",
                    "MY ARG2", (char *)0, env_list) < 0) {
            printf("execle %s error: %m\n", current_dir);
            return 1;
        }
    }

    if (waitpid(pid, NULL, 0) < 0) {
        printf("waitpid %d error: %m\n", pid);
        return 1;
    }

    if ((pid = fork()) < 0) {
        printf("fork again error\n");
        return 1;
    }
    else if (pid == 0) {    /* child */
        /* 书中,下面的语句写的是:
         * if (execlp("echoall", "only 1 arg", (char *)0) < 0)
         * 但由于我的PATH环境变量没有包含当前目录,照着书中写,执行会报错,
         * 所以需要写成下面的形式.下面的形式和书中所写形式的区别是:
         * 1. If filename contains a slash, it is taken as a pathname.
         * 2. Otherwise, the executable file is searched for in the the
         * directories specified by the PATH environment variable.
         *
         * 即,如果execlp()或者execvp()函数的第一个参数filename中包含斜线/,
         * 则这个filename被看作路径名,如果所给路径是绝对路径,则从根目录开始
         * 查找这个路径名指定的文件,如果所给路径是相对路径,则从当前目录开始
         * 查找这个路径名指定的文件.如下面的./echoall包含了一个斜线/,且这个
         * 斜线没有位于路径名最开始的地方,这是一个相对路径,execlp()函数将在
         * 当前目录下查找名为echoall的文件,也只会在当前目录下查找该文件.
         *
         * 反之,如果execlp()或者execvp()函数的第一个参数filename中没有包含
         * 任何斜线/,则它是一个文件名,这两个函数会将PATH中包含的路径前缀挨
         * 个加到这个文件名前面,得到一个路径名,并去查找这个路径名指定的文件
         * 是否存在,如果存在则结束查找,否则继续寻找,直到遍历完所有的路径
         * 前缀为止.例如书中所写的execlp("echoall","only 1 arg", (char *)0)
         * 假设PATH=/bin:/usr/bin,则execlp()会分别按照路径/bin/echoall,
         * 和/usr/bin/echoall 来查找,看这两个路径指定的文件是否存在.如果PATH
         * 中没有包含当前目录,execlp()是不会在当前目录下查找echoall文件的.
         *
         * 总体来说,第一点和第二点的区别是,第二点通过增加PATH中包含的路径
         * 前缀到filename的前面,来得到一个或多个路径名.而第一点直接指定了
         * 路径名.它们最终都是通过路径名来查找文件.因为一个文件的所在正是
         * 通过路径名来指定的.
         */
        if (execlp("./echoall", "only 1 arg", (char *)0) < 0) {
            printf("execlp echoall error: %m\n");
            return 1;
        }
    }

    if (waitpid(pid, NULL, 0) < 0) {
        printf("waitpid %d error: %m\n", pid);
        return 1;
    }

    if ((pid = fork()) < 0) {
        printf("fork error\n");
        return 1;
    }
    else if (pid == 0) {     /* child */
        /* 增加当前目录到PATH环境变量中,这样写为execlp("echoall",...),执行
         * 时就能找到当前目录下的echoall文件,不会报错.
         * 书中对 PATH 的描述如下:
         * The PATH variable contains a list of directories, called path
         * prefixes, that are separated by colons. A zero-length prefix
         * means the current directory. It can be specified as a colon at
         * the beginning of the value, two colons in a row, or a colon at
         * the end of the value. 下面设置 PATH 的值时,就利用了这个特性,
         * 在原先的PATH后面添加一个冒号:,表示增加当前目录到PATH中,这个语句
         * 也可以写为 export PATH=$PATH:. 显式的使用 . 来表示当前目录.
         */
        system("export PATH=$PATH:");
        if (execlp("echoall", "ARG1", "ARG2", (char *)0) < 0) {
            printf("execlp echoall error: %m\n");
            return 1;
        }
    }

    if (waitpid(pid, NULL, 0) < 0) {
        printf("waitpid error\n");
        return 1;
    }

    return 0;
}
