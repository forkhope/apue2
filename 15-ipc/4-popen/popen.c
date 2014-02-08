#include <string.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>     /* _exit() */
#include <stdio.h>
#include <errno.h>

#ifdef OPEN_MAX
static long openmax = OPEN_MAX
#else
static long openmax = 0;
#endif /* OPEN_MAX */

#define OPEN_MAX_GUESS 256

int open_max(void)
{
    if (openmax == 0) {
        errno = 0;

        /* man sysconf 中对该函数的返回值描述如下,其中name指的是传入的参数名
         * If name is invalid, -1 is returned, and errno is set to EINVAL.
         * Otherwise, the value returned is the value of the system resource
         * and errno is not changed. In the case of options, a positive
         * value is returned if a queried option is available, and -1 if it
         * is not. In the case of limits, -1 means that there is no definite
         * limit. 即,当返回值是-1,但是errno的值没变时,表示该系统资源没有被
         * 明确的限制, definite 的意思是 "明确的,一定的". 下面的判断处理了
         * 这种情况,此时会将 openmax 设为一个猜想的值,OPEN_MAX_GUESS,即 256
         */
        if ((openmax = sysconf(_SC_OPEN_MAX)) < 0) {
            if (errno == 0)
                openmax = OPEN_MAX_GUESS;
            else {
                printf("sysconf _SC_OPEN_MAX error: %m\n");
                exit(1);
            }
        }
    }
    return openmax;
}

/* Pointer to array allocated at run-time. */
static pid_t *childpid = NULL;
static int maxfd;

FILE *popen_l(const char *cmd, const char *type)
{
    int i, pfd[2];
    FILE *fp;
    pid_t pid;

    /* only allow "r" or "w" */
    if ((type[0] != 'r' && type[0] != 'w') || type[1] != '\0') {
        errno = EINVAL; /* required by POSIX */
        return NULL;
    }

    if (childpid == NULL) {     /* first time through */
        /* allocate zeroed out array for child pids */
        maxfd = open_max();
        if ((childpid = calloc(maxfd, sizeof(childpid))) == NULL) {
            return NULL;
        }
    }

    if (pipe(pfd) < 0)
        return NULL;    /* errno set by pipe() */

    if ((pid = fork()) < 0) {
        return NULL;    /* errno set by fork() */
    }
    else if (pid == 0) {        /* child */
        if (type[0] == 'r') {
            close(pfd[0]);
            if (pfd[1] != STDOUT_FILENO) {
                dup2(pfd[1], STDOUT_FILENO);
                close(pfd[1]);
            }
        }
        else {
            close(pfd[1]);
            if (pfd[0] != STDIN_FILENO) {
                dup2(pfd[0], STDIN_FILENO);
                close(pfd[0]);
            }
        }

        /* close all descriptors in childpid[].
         * 书中提到, POSIX.1 requires that popen() close any streams that
         * are still open in the child from previous call to popen(). To
         * do this, we go through the childpid array in the child, closing
         * any descriptors that are still open.
         * POSIX.1标准中,对这个要求的原始描述是: The popen() function shall
         * ensure that any streams from previous popen() calls that remain
         * open in the parent process are closed in the new child process.
         * 即,假设已经调用过一次popen(),父进程得到一个FILE *类型的fp指针,然
         * 后父进程再调用popen()时,popen()创建的子进程也拥有一个fp指针,但是
         * 子进程不需要该fp指针,所以子进程要关闭它.
         */
        for (i = 0; i < maxfd; ++i)
            if (childpid[i] > 0)
                close(i);

        /* sh 命令的 -c 选项表示: Read commands from the command_string
         * operand nsteand of from the standard input.
         */
        execl("/bin/sh", "sh", "-c", cmd, (char *)0);
        _exit(127);
    }

    /* parent continues... */
    if (type[0] == 'r') {
        close(pfd[1]);
        if ((fp = fdopen(pfd[0], type)) == NULL)
            return NULL;
    }
    else {
        close(pfd[0]);
        if ((fp = fdopen(pfd[1], type)) == NULL)
            return NULL;
    }

    childpid[fileno(fp)] = pid; /* remember child pid for this fd */
    return fp;
}

/* POSIX.1 标准对 pclose() 的描述如下:
 * The pclose() function shall close a stream that was open by popen(), wait
 * for the command to terminate, and return the termination status of the
 * process that was running the command language interpreter. However, if
 * a call caused the termination status to be unavailable to pclose(), then
 * pclose() shall return -1 with errno set to ECHILD to report this
 * situation. This can happen if the application calls one of the following
 * functions:
 * (1) wait(); (2) waitpid() with a pid argument less than or equal to 0 or
 * equal to the process ID of the command line interpreter. (3) Any other
 * function not defined in this volume of POSIX.1 that could do one of the
 * above.
 * In any case, pclose() shall not return before the child process created
 * by popen() has terminated.
 */
int pclose_l(FILE *fp)
{
    int fd, stat;
    pid_t pid;

    if (childpid == NULL) {
        errno = EINVAL;
        return (-1);    /* popen() has never been called */
    }

    fd = fileno(fp);
    if ((pid = childpid[fd]) == 0) {
        errno = EINVAL;
        return -1;      /* fp wasn't opened by popen() */
    }

    childpid[fd] = 0;
    if (fclose(fp) == EOF)
        return -1;

    /* What happens if the caller of pclose() has established a signal
     * handler of SIGCHLD? The call to waitpid() from pclose() would return
     * an error of EINTR. Since the caller is allowed to catch this signal
     * (or any other signal that might interrupt the call to waitpid()), we
     * simply call waitpid() again if it is interrupted by a caught signal.
     * Note that if the application calls waitpid() and obtains the exit
     * status of the child created by popen(), we will call waitpid() when
     * the application calls pclose, find that the child no longer exists,
     * and return -1 with errno set to ECHILD. This is behavior required by
     * POSIX.1 in this situation. 见 pclose_l() 函数头注释中的描述.
     */
    while (waitpid(pid, &stat, 0) < 0)
        if (errno != EINTR)
            return -1;  /* error other than EINTR fro waitpid() */

    return stat;        /* return child's termination status */
}

#define PAGER "${PAGER:-more}"

int main(int argc, char *argv[])
{
    char line[1024];
    FILE *fpin, *fpout;

    if (argc != 2) {
        printf("usage: %s <filename>\n", argv[0]);
        exit(1);
    }

    if ((fpin = fopen(argv[1], "r")) == NULL) {
        printf("fopen %s error: %s\n", argv[1], strerror(errno));
        exit(1);
    }

    if ((fpout = popen_l(PAGER, "w")) == NULL) {
        printf("popen_l error: %s\n", strerror(errno));
        exit(1);
    }

    while (fgets(line, 1024, fpin) != NULL) {
        if (fputs(line, fpout) == EOF) {
            printf("fputs error: %s\n", strerror(errno));
            exit(1);
        }
    }
    if (ferror(fpin)) {
        printf("fgets error: %s\n", strerror(errno));
        exit(1);
    }

    if (pclose_l(fpout) < 0) {
        printf("pclose_l error: %s\n", strerror(errno));
        exit(1);
    }
    exit(0);
}
