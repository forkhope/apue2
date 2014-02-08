#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <termios.h>
#include <sys/ioctl.h>      /* ioctl() */

static void pr_winsize(int fd)
{
    struct winsize size;

    if (ioctl(fd, TIOCGWINSZ, (char *)&size) < 0) {
        perror("iotcl TIOCGWINSZ error");
        return;
    }

    printf("%d rows, %d columns\n", size.ws_row, size.ws_col);
}

/* 接收到 SIGWINCH 信号,表示终端窗口大小发生了改变,打印当前的窗口大小 */
static void sig_winch(int signum)
{
    printf("SIGWINCH received\n");
    pr_winsize(STDIN_FILENO);
}

/* Most UNIX systems provide a way to keep track of the current terminal
 * window size and to have the kernel notify the foreground process group
 * when the size changes. The kernel maintains a winsize structure for
 * every terminal and pseudo terminal:
 * struct winsize {
 *      unsigned short ws_row;  // rows, in characters
 *      unsigned short ws_col;  // columns, in characters
 *      unsigned short ws_xpixel; // horizontal size, pixels (unused)
 *      unsigned short ws_ypixel; // vertical size, pixels (unused)
 * };
 * The rules for this structure are as follows.
 * 1. We can fetch the current value of this structure using an ioctl() of
 * TIOCGWINSZ.
 * 2. We can store a new value of this structure in the kernel using an
 * ioctl() of TIOCSWINSZ. If this new value differs from the current value
 * stored in the kernel, a SIGWINCH signal is sent to the foreground process
 * group. (The default action for this signal is to be ignored.)
 * 3. Other than storing the current value of the structure and generating
 * a signal when the value changes, the kernel does nothing else with this
 * structure. Interpreting the structure is entirely up to the application.
 *
 * The reason for providing this feature is to notify applications (such as
 * the vi editor) when the window size changes. When it receives the signal,
 * the application can fetch the new size and redraw the screen.
 *
 * The following program prints the current window size and goes to sleep.
 * Each time the window size changes, SIGWINCH is caught and the new size
 * is printed. We have to terminate this program with a signal.
 */
int main(void)
{
    struct sigaction sig;

    if (isatty(STDIN_FILENO) == 0)
        return 1;

    sig.sa_handler = sig_winch;
    sigemptyset(&sig.sa_mask);
    sig.sa_flags = 0;
    if (sigaction(SIGWINCH, &sig, NULL) < 0) {
        perror("sigaction SIGWINCH error");
        return 1;
    }

    pr_winsize(STDIN_FILENO);       /* print initial size */
    for (;;)                        /* and sleep forever */
        pause();
    return 0;
}
