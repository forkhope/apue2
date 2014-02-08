#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

/* The signal handler is invoked when the program writes to the pipe
 * that has no reader.
 */
static void sig_pipe(int arg)
{
    printf("SIGPIPE caught\n");
    exit(1);
}

/* A UNIX system filter is a program that reads from standard input and
 * writes to standard output. Filters are normally conntected lineayly in
 * shell pipelines. A filter becomes a coprocess when the same program
 * generates the filter's input and reads the filters' output. 即, filter
 * 的概念比 coprocess 的广泛.
 * Whereas popen() gives us a one-way pipe to the standard input or from
 * the standard output of another process, with a coprocess, we have two
 * one-way pipes to the other process: one to its standard input and one
 * from its output.
 */
int main(void)
{
    int n, fd1[2], fd2[2];
    char line[1024];
    struct sigaction sa;
    pid_t pid;

    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sa.sa_handler = sig_pipe;
    if (sigaction(SIGPIPE, &sa, NULL) < 0) {
        printf("sigaction SIGPIPE error: %m\n");
        exit(1);
    }

    if (pipe(fd1) < 0 || pipe(fd2) < 0) {
        printf("creat pipe error: %m\n");
        exit(1);
    }

    if ((pid = fork()) < 0) {
        printf("can't fork: %m\n");
        exit(1);
    }
    else if (pid > 0) {         /* parent */
        close(fd1[0]);
        close(fd2[1]);
        while (fgets(line, 1024, stdin) != NULL) {
            n = strlen(line);
            if (write(fd1[1], line, n ) != n) {
                printf("parent, can't write fd1[1]: %m\n");
                exit(1);
            }
            if ((n = read(fd2[0], line, 1024)) > 0) {
                line[n] = '\0';         /* null terminate */
                if (fputs(line, stdout) == EOF) {
                    printf("parent: fputs error: %m\n");
                    exit(1);
                }
            }
            else if (n == 0) {
                printf("child closed pipe\n");
                break;
            }
            else {
                printf("read fd2[0] error: %m\n");
                exit(1);
            }
        }
        if (ferror(stdin)) {
            printf("parent fgets error on stdin: %m\n");
            exit(1);
        }
    }
    else {      /* child */
        close(fd1[1]);
        close(fd2[0]);
        if (fd1[0] != STDIN_FILENO) {
            if (dup2(fd1[0], STDIN_FILENO) != STDIN_FILENO) {
                printf("dup2 fd1[0] to STDIN_FILENO error: %m\n");
                exit(1);
            }
            close(fd1[0]);      /* 不要忘记关闭这个不再需要的描述符 */
        }
        if (fd2[1] != STDOUT_FILENO) {
            if (dup2(fd2[1], STDOUT_FILENO) != STDOUT_FILENO) {
                printf("dup2 fd2[1] to STDOUT_FILENO error: %m\n");
                exit(1);
            }
            close(fd2[1]);
        }

        if (execl("add2", "add2", (char *)0) < 0)
            _exit(127);
    }
    exit(0);
}
