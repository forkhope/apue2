#include <stdio.h>
#include <unistd.h>     /* isatty() */
#include <termios.h>

/* 终端I/O的特殊字符 ---- DSUSP
 * The delayed-suspend job-control character. This character is recognized
 * on input in extended mode (IEXTEN) if job control is supported and if
 * the ISIG flag is set. Like the SUSP character, this delayed-suspend
 * character generates the SIGTSTP signal that is sent to all processes in
 * the foreground process group. But the delayed-suspend character generates
 * a signal only when a process reads from the controlling terminal, not
 * when the character is typed. This character is discarded when processed
 * (i.e., it is not passed to the process).
 * man tcgetattr 手册中提到: VDSUSP (not in POSIX; not supported under
 * Linux; 031, EM, Ctrl-Y). 这里面的031是该字符的值,八进制.
 * 注意,这里说的不支持,是说不支持该字符对应的功能,并不代表不支持VDSUSP这个
 * 宏,虽然这个宏在Linux中确实不支持,但是man手册中同样提到不支持VDISCARD,但
 * VDISCARD这个宏还是定义了的.
 */
int main(void)
{
    struct termios term;

    if (isatty(STDIN_FILENO) == 0) {
        perror("STDIN_FILENO is not a terminal");
        return 1;
    }

    if (tcgetattr(STDIN_FILENO, &term) < 0) {
        perror("tcgetatte STDIN_FILENO error");
        return 1;
    }

    /* 这一句会报错:error: 'DSUSP' undeclared (first use in this function)
     * 即Linux中,都没有定义VDSUSP这个宏,而虽然man手册中,都提到VDISCARD和
     * VDSUSP在Linux中不支持,但是Linux中还是定义了VDISCARD宏,而VDSUSP宏却
     * 没有被定义.
     */
    // printf("term.c_cc[VDSUSP]: %d\n", term.c_cc[VDSUSP]);
#ifdef VDSUSP
    printf("term.c_cc[VDSUSP]: %d\n", term.c_cc[VDSUSP]);
#else
    printf("VDSUSP not in POSIX; not supported under Linux\n");
#endif /* VDSUSP */

#ifdef VDISCARD
    printf("term.c_cc[VDISCARD]: %d\n", term.c_cc[VDISCARD]);
#else
    printf("VDISCARD not in POSIX; not supported under Linux\n");
#endif /* VDISCARD */

    return 0;
}
