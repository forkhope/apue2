#include <stdio.h>
#include <unistd.h>             /* getpass() */
#include <termios.h>
#include <signal.h>

#define MAX_PASS_LEN    (8)     /* max #chars for user to enter */
static char buf[MAX_PASS_LEN + 1];  /* null byte at end */

/* Linux的man手册对 getpass() 函数的描述为:
 * #include <unistd.h>
 * char *getpass(const char *prompt);
 * The function is obsolete. Do not use it. The getpass() function opens
 * /dev/tty (the controlling terminal of the process), outputs the string
 * prompr, turns off echoing, reads one line (the "password"), restores the
 * terminal state and closes /dev/tty again.
 * The function getpass() returns a pointer to a static buffer containing
 * the password without the trailing newline, terminated by a null byte
 * ('\0'). This buffer may be overwritten by a following call. On error, the
 * terminal state is restored, and NULL is returned.
 *
 * 下面是书中对该函数的部分描述和实现.
 * We now show the function getpass(), which reads a password of some type
 * from the user at a terminal. To read the password, the function must turn
 * off echoing, but if can leave the terminal in canonical mode, as whatever
 * we type as the password froms a complete line.
 * There are several points to consider in this example.
 * 1. Instead of hardwiring /dev/tty into the program, we call the function
 * ctermid() to open the controlling terminal.
 * 2. We block the two signals SIGINT and SIGTSTP. If we didn't do this,
 * entering the INTR character would abort the program and leave the
 * terminal with echoing disabled. Similarly, entering the SUSP character
 * would stop the program and return to the shell with echoing disabled. We
 * choose to block the signals while we have echoing disabled. If they are
 * generated while we've reading the password, they are held until we return
 * 3. We use standard I/O to read and write the controlling terminal. We
 * specifically set the stream to be unbuffered; otherwise, there might be
 * some interactions between the writing and reading of the stream (we would
 * need some calls to fflush()). We could have also used unbuffered I/O, buf
 * we would have to simulate the getc() function using read(). 这里说的
 * "unbuffered I/O"指的是read(),write()这类函数,此时需要通过read()函数来实现
 * 一个类似于getc()的函数,因为输入密码时,以便逐个字符判断是否读到了换行符.
 * 4. We store only up to eight characters as the password. Any additional
 * characters that are entered are ignored.
 */
static char *getpass_l(const char *prompt)
{
    char *ptr;
    sigset_t sig, osig;
    struct termios term, oldterm;
    FILE *fp;
    int c;

    if ((fp = fopen(ctermid(NULL), "r+")) == NULL)
        return NULL;

    /* #include <stdio.h>
     * void setbuf(FILE *fp, char *buf);
     * To disable buffering, we set buf to NULL. 即,此时为无缓冲模式.
     */
    setbuf(fp, NULL);

    sigemptyset(&sig);
    sigaddset(&sig, SIGINT);        /* block SIGINT */
    sigaddset(&sig, SIGTSTP);       /* block SIGTSTP */
    sigprocmask(SIG_BLOCK, &sig, &osig);    /* and save mask */

    tcgetattr(fileno(fp), &oldterm);    /* save tty state */
    term = oldterm;                     /* structure copy */
    term.c_lflag &= ~(ECHO | ECHOE | ECHOK | ECHONL);
    tcsetattr(fileno(fp), TCSAFLUSH, &term);
    
    fputs(prompt, fp);
    ptr = buf;
    while ((c = getc(fp)) != EOF && c != '\n')
        if (ptr < &buf[MAX_PASS_LEN])
            *ptr++ = c;
    *ptr = '\0';                    /* null terminate */
    putc('\n', fp);                 /* we echo a newline */

    tcsetattr(fileno(fp), TCSAFLUSH, &oldterm); /* restore TTY state */
    sigprocmask(SIG_SETMASK, &osig, NULL);      /* restore mask */
    fclose(fp);                     /* done with /dev/tty */
    return buf;
}

/* Whenever a program that calls getpss() is done with the cleartext
 * password, the program should zero it out in memory, just to be safe. If
 * the program were to generate a core file that others might be able to
 * read or if some other process were somehow able to read our memory, they
 * might be able to read the cleartext password. (By "cleartext", we mean
 * the password that we type at the prompt that is printed by getpass().
 * Most UNIX system programs then modify this cleartext password into an
 * encrypted password, not the cleartext password.)
 * 下面这个函数就用于清空保存了输入密码的缓冲区.
 */
static void clearpass_l(void)
{
    int i;

    for (i = 0; i < sizeof(buf); ++i)
        buf[i] = 0;
}

/* Canonical mode is simple: we issue a read, and the terminal driver
 * returns when a line has been entered. Several conditions cause the read
 * to return.
 * (1) The read returns when the requested number of bytes have beed read.
 * We don't have to read a complete line, If we read a partial line, no
 * information is lost; the next read starts where the previous read stopped
 * (2) The read returns when a line delimiter is encountered. Recall from
 * Section 18.3 that the following characters are interpreted as end of line
 * in canonical mode: NO, EOL, EOL2, and EOF. Also, recall from Section 18.5
 * that if ICRNL is set and if IGNCR is not set, then the CR character also
 * terminates a line, since it acts just like the NL character.
 * Realize that of these five line delimiters, one (EOF) is discarded by
 * the terminal driver when it's processed. The other four are returned to
 * the caller as the last character of the line.
 * 所以,当已经有输入时,输入EOF会结束一行输入,此时read()函数返回值并不是0,而
 * 是以前读到的字符数,此时再输入EOF,read()函数返回的才是0,也就是这种情况下,
 * 要输入两次EOF,才会结束整个输入.
 * (3) The read also returns if a signal is caught and if the function is
 * not automatically restarted.
 */
int main(void)
{
    char *password;

    if ((password = getpass_l("Enter password:")) == NULL) {
        perror("getpass_l error");
        return 1;
    }
    printf("getpass_l: password: %s\n", password);
    /* zero it out when we're done with the password */
    clearpass_l();

    /* 下面调用Linux自带的getpss()函数,查看它的执行结果 */
    printf("----------- 调用系统函数 getpass() --------\n");
    if ((password = getpass("Enter password:")) == NULL) {
        perror("getpass error");
        return 1;
    }
    printf("getpass: password: %s\n", password);

    return 0;
}
