#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>

/* 终端标志位: ECHOCTL, c_lflag, not in POSIX.1
 * If set and if ECHO if set, ASCII control characters (those characters in
 * the range 0 through octal 37, inclusive) other than the ASCII TAB, the
 * ASCII NL, and the START and STOP characters are echoed as ^X, where X is
 * the character formed by adding octal 100 to the control character. This
 * means that the ASCII Control-A character (octal 1) is echoed as ^A. Also,
 * the ASCII DELETE character (octal 177) is echoed as ^?. If this flag is
 * not set, the ASCII control characters are echoed as themselves. As with
 * the ECHO flag, this flag affects the echoing of control characters in
 * both canonical and noncanonical modes.
 *
 * Be aware that some systems echo the EOF character differently, since its
 * typical value is Control-D. (Control-D is the ASCII EOT character, which
 * can cause some terminals to hangup.) Check your manual.
 */
int main(void)
{
    struct termios term, oldterm;

    if (tcgetattr(STDIN_FILENO, &term) < 0) {
        perror("tcgetattr STDIN_FILENO error");
        return 1;
    }
    oldterm = term;

    printf("term.c_lflag: %#x, ECHOCTL: %#x, &= %#x\n",
            term.c_lflag, ECHOCTL, term.c_lflag & ECHOCTL);

    term.c_lflag &= ~ECHOCTL;
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &term) < 0) {
        perror("tcsetattr STDIN_FILENO error");
        return 1;
    }

    printf("term.c_lflag: %#x, ECHOCTL: %#x, &= %#x\n",
            term.c_lflag, ECHOCTL, term.c_lflag & ECHOCTL);

    /* 关闭 ECHOCTL 标志位后,再输入Ctrl-A,将会显示乱码.而开启该标志位
     * 时,输入Ctrl-A,显示的是 ^A.
     */
    system("cat");

    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &oldterm) < 0) {
        perror("tcsetattr STDIN_FILENO error");
        return 1;
    }
    return 0;
}
