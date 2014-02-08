#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>

/* The shell command ${PAGER:-more} says to use the value of the shell
 * variable PAGER if it is defined and non-null; otherwise, use the
 * string more. popen()函数调用sh -c来执行所给的命令,shell负责扩展该字符串
 */
#define PAGER "${PAGER:-more}"  /* environment variable, or default */

/* Since a command operation is to create a pipe to another process, to
 * either read its output or send it input, the standard I/O library has
 * historically provided the popen() and pclose() function. These two
 * functions handle all the dirty work that we're been doing ourselves:
 * creating a pipe, forking a child, closing the unused ends of the pipe,
 * executing a shell to run the command, and waiting for the command to
 * terminate.
 * #include <stdio.h>
 * FILE *popen(const char *cmdstring, const char *type);
 *      Returns: file pointer if OK, NULL on error
 * int pclose(FILE *fp);
 *      Returns: termination status of cmdstring, or -1 on error
 * The function popen() does a fork() and exec() to execute the cmdstring,
 * and returns a standard I/O file pointer. If type is "r", the file
 * descriptor is connected to the standard output of cmdstring. If type is
 * "w", the file pointer is connected to the standard input of cmdstring.
 * One way to remember the final argument to popen() is to remember that,
 * like fopen(), the returned file pointer is readable if type is "r" or
 * writable if type is "w".
 * The pclose() function closes the standard I/O stream, waits for the
 * command to terminate, and returns the termination status of the shell.
 * If the shell cannot be executed, the termination status returned by
 * pclose() is as if the shell had executed exit(127).
 * The cmdstring is executed by the Bourne shell, as in "sh -c cmdstring".
 * This means that the shell expands any of its special characters in
 * cmdstring.
 */
int main(int argc, char *argv[])
{
    char line[1024];
    FILE *fpin, *fpout;
    int status;

    if (argc != 2) {
        printf("usage: $s <filename>\n", argv[0]);
        exit(1);
    }

    if ((fpin = fopen(argv[1], "r")) == NULL) {
        printf("fopen %s error: %s\n", argv[1], strerror(errno));
        exit(1);
    }

    if ((fpout = popen(PAGER, "w")) == NULL) {
        printf("popen %s error: %s\n", PAGER, strerror(errno));
        exit(1);
    }

    /* copy argv[1] to pager */
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

    fclose(fpin);
    if ((status = pclose(fpout)) == -1) {
        printf("pclose error: %s\n", strerror(errno));
        exit(1);
    }
    printf("pclose return: %d\n", status);

    exit(0);
}
