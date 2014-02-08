#include <stdio.h>
#include <unistd.h>
#include <termios.h>

void print_baudrate(struct termios term)
{
    speed_t ispeed, ospeed;

    ispeed = cfgetispeed(&term);
    ospeed = cfgetospeed(&term);
    printf("ispeed: %d, ospeed: %d\n", ispeed, ospeed);
}

/* Baud Rate Functions
 * The term baud rate is a historical term that should be referred to
 * today as "bits per second." Although most terminal devices use the
 * same baud rate for both input and output, the capability exists to
 * set the two to different values, if the hardware allows this.
 * #include <termios.h>
 * speed_t cfgetispeed(const struct termios *termptr);
 * speed_t cfgetospeed(const struct termios *termptr);
 *      Both return: baud rate value
 * int cfsetispeed(struct termios *termptr, speed_t speed);
 * int cfsetospeed(struct termios *termptr, speed_t speed);
 *      Both return: 0 if OK, -1 on error
 *
 * The return value from the two cfget functions and the speed argument to
 * the two cfset functions are one of the following constants: B50, B75,
 * B100, B134, B150, B200, B300, B600, B1200, B1800, B2400, B4800, B9600,
 * B19200, or B38400. The constant B0 means "hang u." When B0 is specified
 * as the output baud rate when tcsetattr() is called, the modem control
 * lines are no longer asserted.
 * Most system define additional baud rate values, such as B57600 and
 * B115200.
 *
 * To use these functions, we must realize that the input and output baud
 * retes are stored in the deice's termios structure. Before calling either
 * or the cfget functions, we first have to obtain the device's termios
 * structure using tcgetattr(). Similarly, after calling either of the two
 * cfset functions, all we've done is set the baud rate in a termios
 * structure. For this change to affect the device, we have to call
 * tcsetattr(). If there is an error in either of the baud rates that we
 * set, we may not find out about the error until we call tcsetattr().
 *
 * The four baud rate functions exist to insulate applications from
 * differences in the way that implementations represent baud rates in
 * the termios structure.
 */
int main(void)
{
    struct termios term, oldterm;
    speed_t ispeed, ospeed;

    if (tcgetattr(STDIN_FILENO, &term) < 0) {
        perror("tcgetattr STDIN_FILENO error");
        return 1;
    }
    oldterm = term;

    printf("B38400: %d\n", B38400);
    print_baudrate(term);

    ispeed = B115200;
    if (cfsetispeed(&term, ispeed) < 0) {
        perror("cfsetispeed error");
        return 1;
    }

    ospeed = B115200;
    if (cfsetospeed(&term, ospeed) < 0) {
        perror("cfsetospeed error");
        return 1;
    }

    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &term) < 0) {
        perror("tcsetattr STDIN_FILENO error");
        return 1;
    }

    printf("B115200: %d\n", B115200);
    print_baudrate(term);

    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &oldterm) < 0) {
        perror("tcsetattr STDIN_FILENO error");
        return 1;
    }
    return 0;
}
