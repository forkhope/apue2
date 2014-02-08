#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>

/* The following four functions provide line control capability for
 * terminal devices. All four require that filedes refer to a terminal
 * device; otherwise, an error is returned with errno set to ENOTTY.
 * #include <termios.h>
 * int tcdrain(int fd);
 * int tcflow(int fd, int action);
 * int fcflush(int fd, int queue);
 * int tcsendbreak(int fd, int duration);
 *      All four return: 0 if OK, -1 on error
 * The tcdrain() function waits for all output to be transmitted. The
 * fcflow() function gives us control over both input and output flow
 * control. The action argument must be one of the following four values:
 * TCOOF    Output is suspended.
 * TCOON    Output that was previously suspended is restarted.
 * TCIOFF   The system transmits a STOP character, which should cause the
 *          terminal device to stop sending data.
 * TCION    The system transmits a START character, which should cause the
 *          terminal device to resume sending data.
 * The tcflush() function lets us flush (throw away) either the input buffer
 * (data that has received by the terminal driver, which we have not read)
 * or the output buffer (data that we have written, which has not yet been
 * transmitted). The queue argument must be one of the following three
 * constants:
 * TCIFLUSH: The input queue is flushed.
 * TCOFLUSH: The output queue is flushed.
 * TCIOFLUSH: Both the input and output queues are flushed.
 * The tcsendbreak() function transmits a continuous stream of zero bits
 * for a specified duration. If the duration argument is 0, the transmission
 * lasts between 0.25 seconds and 0.5 seconds. POSIX.1 specifies that if
 * duration is nonzero, the transmission time is implementation dependent.
 */
int main(void)
{
    if (tcdrain(STDOUT_FILENO) < 0) {
        perror("tcdrain STDOUT_FILENO error");
        return 1;
    }
    printf("tcdrain STDOUT_FILENO success\n");

    /* 经过验证,执行下面这条语句,终端会停止输出,光标不动,之后不管
     * 输入什么都无法恢复, Ctrl-C, Ctrl-Q, Ctrl-Q等都没用.
     */
    // if (tcflow(STDOUT_FILENO, TCOOFF) < 0) {
    //     perror("tcflow STDOUT_FILENO TCOOFF error");
    //     return 1;
    // }
    printf("tcflow STDOUT_FILENO TCOOFF success\n");

    if (tcflow(STDOUT_FILENO, TCOON) < 0) {
        perror("tcflow STDOUT_FILENO TCOON error");
        return 1;
    }
    printf("tcflow STDOUT_FILENO TCOON success\n");

    /* 执行完这条语句后,终端会打印一个乱码的字符,应该就是STOP字符 */
    if (tcflow(STDOUT_FILENO, TCIOFF) < 0) {
        perror("tcflow STDOUT_FILENO TCIOFF error");
        return 1;
    }
    putchar('\n');
    printf("tcflow STDOUT_FILENO TCIOFF success\n");

    /* 执行完这条语句后,终端会打印一个乱码的字符,应该就是START字符 */
    if (tcflow(STDOUT_FILENO, TCION) < 0) {
        perror("tcflow STDOUT_FILENO TCION error");
        return 1;
    }
    putchar('\n');
    printf("tcflow STDOUT_FILENO TCION success\n");

    if (tcflush(STDIN_FILENO, TCIFLUSH) < 0) {
        perror("tcflush STDIN_FILENO TCIFLUSH error");
        return 1;
    }
    printf("tcflush STDIN_FILENO TCIFLUSH success\n");

    if (tcflush(STDOUT_FILENO, TCOFLUSH) < 0) {
        perror("tcflush STDOUT_FILENO TCOFLUSH error");
        return 1;
    }
    printf("tcflush STDOUT_FILENO TCOFLUSH success\n");

    if (tcsendbreak(STDOUT_FILENO, 4) < 0) {
        perror("tcsendbreak STDOUT_FILENO 4 error");
        return 1;
    }
    printf("tcsendbreak STDOUT_FILENO 4 success\n");

    return 0;
}
