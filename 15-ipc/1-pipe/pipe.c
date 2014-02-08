#include <stdio.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>

/* Pipes are the oldest form of UNIX System IPC and are provided by all
 * UNIX systems. Pipes have two limitations.
 * 1. Historically, they have been half deplex (i.e., data flows in only
 * one direction). Some systems now provide full-duplex pipes, but for
 * maximum portability, we should never assume that this is the case.
 * 2. Pipe can be used only between processes that have a common ancestor.
 * Normally, a pipe is created by a process, that process calls fork(), and
 * the pipe is used between the parent and the child.
 * A pipe is created by calling the pipe() function.
 * #include <unistd.h>
 * int pipe(int pipefd[2]);
 *      Returns: 0 if OK, -1 on error.
 * Two file descriptors are returned through the pipefd argument: pipefd[0]
 * is open for reading, and pipefd[1] is open for writing. The output of
 * pipefd[1] is the input for pipe[0]. Data written to the write end of the
 * pipe (pipefd[1]) is buffered by the kernel until it is read from the read
 * end of the pipe (pipefd[0]). 即,从pipefd[0]读到的数据是被写入pipefd[1]的
 * 数据.数据是缓存在内核的pipe文件中.
 * The fstat() function returns a file type of FIFO for the file descriptor
 * of either end of a pipe. We can test for a pipe with the S_ISFIFO macro.
 * POSIX.1 states that the st_size member of the stat structure is undefined
 * for pipes. 一些实现可能会在管道读端的st_size中存放当前可读的字节数.
 * When one end of a pipe is closed, the following two rules apply.
 * 1. If we read() from a pipe whose write end has been closed, read()
 * returns 0 to indicate an end of file after all the data has been read.
 * (Technically, we should say that this end of file is not generated until
 * there are no more writers for the pipe.)
 * 2. If we write() to a pipe whose read end has been closed, the signal
 * SIGPIPE is generated. If we either ignore the signal or cather is and
 * return from the signal handler, write() return -1 with errno set to EPIPE
 *
 * When we're writing to a pipe (or FIFO), the constant PIPE_BUF specifies
 * the kernel's pipe buffer size. We can determine the value of PIPE_BUF by
 * using pathconf() or fpathconf().
 */
int main(void)
{
    int pipefd[2];
    pid_t pid;
    int n;
    char buf[1024];

    if (pipe(pipefd) < 0) {
        printf("pipe error: %s\n", strerror(errno));
        exit(1);
    }

    if ((pid = fork()) < 0) {
        printf("fork error: %s\n", strerror(errno));
        exit(1);
    }
    else if (pid > 0) {         /* parent */
        close(pipefd[0]);
        if (write(pipefd[1], "Hello, world\n", 13) != 13) {
            printf("write to pipefd[1] error: %s\n", strerror(errno));
            exit(1);
        }

        close(pipefd[1]);
        if (waitpid(pid, NULL, 0) < 0) {
            printf("waitpid error: %s\n", strerror(errno));
            exit(1);
        }
    }
    else {      /* child */
        close(pipefd[1]);
        if ((n = read(pipefd[0], buf, 1024)) < 0) {
            printf("read error: %s\n", strerror(errno));
            exit(1);
        }

        if (write(STDOUT_FILENO, buf, n) != n) {
            printf("write STDOUT_FILENO error: %s\n", strerror(errno));
            exit(1);
        }
        close(pipefd[0]);
    }
    exit(0);
}
