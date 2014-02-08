#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

#define PAGER_DEF "/bin/more"   /* default pager program */

/* In the prevous example, we called read() and write() directly on the
 * pipe descriptors. What is more interesting is to duplicate the pipe
 * descriptors onto standard input or standard output. Often, the child
 * then runs some other program, and that program can either read from its
 * standard input (the pipe that we created) or write to its standard
 * output (the pipe).
 */
int main(int argc, char *argv[])
{
    int n;
    int pipefd[2];
    char *pager, *argv0;
    char buf[1024];
    FILE *fp;
    pid_t pid;

    if (argc != 2) {
        printf("usage: %s: <filename>\n", argv[0]);
        exit(1);
    }

    if ((fp = fopen(argv[1], "r")) == NULL) {
        printf("fopen %s: error: %s\n", argv[1], strerror(errno));
        exit(1);
    }

    if (pipe(pipefd) < 0) {
        printf("pipe error: %s\n", strerror(errno));
        exit(1);
    }

    if ((pid = fork()) < 0) {
        printf("fork error: %s\n", strerror(errno));
        exit(1);
    }
    else if (pid > 0) {         /* parent */
        close(pipefd[0]);       /* close read end */

        /* parent copies argv[1] to pipe */
        while (fgets(buf, 1024, fp) != NULL) {
            n = strlen(buf);
            if (write(pipefd[1], buf, n) != n) {
                printf("write error: %s\n", strerror(errno));
                exit(1);
            }
        }
        if (ferror(fp)) {
            printf("fgets error: %s\n", strerror(errno));
            exit(1);
        }
        
        close(pipefd[1]);       /* close write end of pipe for reader */

        if (waitpid(pid, NULL, 0) < 0) {
            printf("waitpid error: %s\n", strerror(errno));
            exit(1);
        }
    }
    else {      /* child */
        close(pipefd[1]);       /* close write end */
        /* 在开始的时候,下面写错为STDOUT_FILENO,导致执行结果出错.对于管道
         * 来将,还是要明确,pipefd[1]是写入端,pipefd[0]是读取端,那么读取端
         * 应该是接收输入的,所以它应该关联到标准输入,表示读取标准输入的内容
         * The child calls dup2() to have its standard input be the read
         * end of the pipe. When the pager program is executed, its standard
         * input will be the read end of the pipe.
         * When we duplicate a descriptor onto another (pipefd[0] onto
         * standard input in the child), we have to be careful that the
         * descriptor doesn't already have the desired value. If the
         * descriptor already had the desired value and we called dup2()
         * and close(), the single copy of the descriptor would be closed.
         * Nevertheless, whenever we call dup2() and close() to duplicate
         * a descriptor onto another, we'll always compare the descriptors
         * first, as a defensive programming measure.
         */
        if (pipefd[0] != STDIN_FILENO) {
            if (dup2(pipefd[0], STDIN_FILENO) != STDIN_FILENO) {
                printf("dup2 error: %s\n", strerror(errno));
                exit(1);
            }
            close(pipefd[0]);   /* don't need this after dup2() */
        }

        /* Note how we try to use the environment variable PAGER to obtain
         * the name of the user's pager program. If this doesn't work, we
         * use a default. This is a common usage of environment variables.
         *
         * get arguments for execl().
         */
        if ((pager = getenv("PAGER")) == NULL) {
            pager = PAGER_DEF;
        }

        /* 由于一般来说,程序的argv[0]只包括程序本身的名字,而不包括完整的
         * 路径,所以下面过滤pager命令前面的路径,只保留最后的命令名本身.
         */
        if ((argv0 = strrchr(pager, '/')) != NULL) 
            ++argv0;            /* step past rightmost slash */
        else
            argv0 = pager;      /* no slash in pager */

        if (execl(pager, argv0, (char *)0) < 0) {
            printf("execl error: %s\n", strerror(errno));
            exit(1);
        }
    }
    exit(0);
}
