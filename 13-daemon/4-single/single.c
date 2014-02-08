#include <stdio.h>
#include <signal.h>
#include <syslog.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/resource.h>
#include <unistd.h>

int lockfile(int fd)
{
    struct flock fl;

    fl.l_type = F_WRLCK;
    fl.l_start = 0;
    fl.l_whence = SEEK_SET;
    fl.l_len = 0;
    return(fcntl(fd, F_SETLK, &fl));
}

/* 由于守护进程会执行chdir("/")语句,将工作目录移到根目录,所以下面的路径
 * 要写成绝对路径,之前写为 #define LOCKFILE "daemon.pid",然后以普通用户
 * 运行时,open()函数一直报错: can't open daemon.pid: Permission denied
 * 用超级用户运行就没有这个问题,一开始怀疑是守护进程本身一些特性导致的,
 * 后来突然想到这一点,守护进程已经将工作目录移到了根目录,则单纯只写
 * daemon.pid的话,会在根目录下创建该文件,由于普通用户不能在根目录下创建
 * 文件,才会一直报错.那么之前用超级用户执行该程序时,应该已经在根目录下
 * 创建了daemon.pid文件,cd到根目录下面查看了一下,确实如此,证明猜测无误.
 * 书中该路径写的是/var/run/daemon.pid,由于我不想每次执行这个程序,都要输
 * 入密码,就将路径改为了daemon.pid,以为会在程序源文件所在目录生成daemon.pid,
 * 就不需要输入密码了,没想到之前执行了好几次,一直都报错"Permission denied",
 * 把自己都搞糊涂了.分析之后,发现是上面所描述的原因,也算是意外收获了.
 */
#define LOCKFILE "/home/john/program/apue2/13-daemon/4-single/daemon.pid"
#define LOCKMODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)

/* This function illustrates the use of file and record locking to ensure
 * that only one copy of a daemon is running. Each copy of the daemon will
 * try to create a file and write its process ID in it. If the file is
 * already locked, the lockfile() function will fail with errno set to
 * EACCESS or EAGAIN, so we return 1, indicating that the daemon is already
 * running. Otherwise, we truncate the file, write our process ID to it,
 * and return 0.
 */
int already_running(void)
{
    int fd;
    char buf[16];

    if ((fd = open(LOCKFILE, O_RDWR | O_CREAT, LOCKMODE)) < 0) {
        /* 之前提过,守护进程可通过syslog()来打印错误信息,由于上面的
         * daemonize()函数将标准输出关联到/dev/null,直接调用Printf()函数
         * 不会在控制台上产生任何输出.
         */
        syslog(LOG_ERR, "can't open %s: %s", LOCKFILE, strerror(errno));
        exit(1);
    }
    if (lockfile(fd) < 0) {
        printf("enter lockfile(fd) < 0\n");
        if (errno == EACCES || errno == EAGAIN) {
            close(fd);
            return 1;
        }
        syslog(LOG_ERR, "can't lock %s: %s", LOCKFILE, strerror(errno));
        exit(1);
    }
    ftruncate(fd, 0);
    sprintf(buf, "%ld", (long)getpid());
    /* 不要忘了第三个参数是strlen(buf)+1,而不是strlen(buf),以便写入最后
     * 的'\0'结束符.
     */
    write(fd, buf, strlen(buf)+1);
    return 0;
}

void daemonize(const char *cmd)
{
    int i, fd0, fd1, fd2;
    pid_t pid;
    struct rlimit rl;
    struct sigaction sa;

#if 0
    /* 之前在daemonize()函数的开头执行already_running()函数,结果发现这个
     * 函数没有起到作用,还是可以创建多个进程实例,在网上找了一下,简单的
     * 解释是: 子进程不继承父进程的进程锁、文件锁、数据锁等.所以,在
     * daemonize()函数的开头执行already_running()函数时,其对daemon.pid文件
     * 加的锁并不被最终的守护进程所继承,该锁被丢弃了,导致lockfile()函数没有
     * 起到应有的作用.修改方法就是在最终创建的子进程那里来执行
     * already_running()函数,如后面的代码所示.
     */
    if (already_running() == 1) {
        printf("%s: already runing...\n", cmd);
        return;
    }
#endif /* 0 */

    umask(0);

    if (getrlimit(RLIMIT_NOFILE, &rl) < 0) {
        printf("%s: can't get file limit: %s\n", cmd, strerror(errno));
        exit(1);
    }

    if ((pid = fork()) < 0) {
        printf("%s: fork error: %s\n", cmd, strerror(errno));
        exit(1);
    }
    else if (pid != 0)  /* parent */
        exit(0);
    setsid();

    sa.sa_handler = SIG_IGN;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    if (sigaction(SIGHUP, &sa, NULL) < 0) {
        printf("%s: can't ignore SIGHUP: %s\n", cmd, strerror(errno));
        exit(1);
    }
    if ((pid = fork()) < 0) {
        printf("%s: fork again error: %s\n", cmd, strerror(errno));
        exit(1);
    }
    else if (pid != 0)  /* parent */
        exit(0);

    if (chdir("/") < 0) {
        printf("%s: can't change directory to /: %s\n",cmd,strerror(errno));
        exit(1);
    }

    if (rl.rlim_max == RLIM_INFINITY)
        rl.rlim_max = 1024;
    for (i = 0; i < rl.rlim_max; ++i)
        close(i);

    fd0 = open("/dev/null", O_RDWR);
    fd1 = dup(fd0);
    fd2 = dup(fd0);

    openlog(cmd, LOG_CONS | LOG_PID, LOG_USER);

    /* 在最终子进程里面来执行already_running()函数,确保守护进程拥有
     * daemon.pid文件的write lock.
     */
    if (already_running() == 1) {
        syslog(LOG_ERR, "%s: already_running, this one will exit", cmd);
        exit(1);
    }

    if (fd0 != 0 || fd1 != 1 || fd2 != 2) {
        syslog(LOG_ERR, "unexpected file descriptors: %d %d %d",
                fd0, fd1, fd2);
        exit(1);
    }

    sleep(10);
}

/* Some daemons are implemented so that only a single copy of the daemon
 * should be running at a time for proper operation. The daemon might need
 * exclusive access to a device, for example.
 * The file- and record-locking mechanism provides the basis for one way to
 * ensure that only one copy of a daemon is running. If each daemon creates
 * a file and places a write lock on the entire file, only one such write
 * lock will be allowed to be created. Successive attempts to create write
 * locks will fail, serving as an indication to successive copies of the
 * daemon that another instance is already running.
 * File and record locking provides a convenient mutual-exclusion mechanism.
 * If the daemon obtains a write-lock on an entire file, the lock will be
 * removed automatically if the daemon exits. This simplifies recovery,
 * removing the need for us to clean up from the previous instance of the
 * daemon.
 */
int main(void)
{
    daemonize("test");
    /* 书中13.7小节的例子中,对already_running()函数的用法如下所示,是写在
     * daemonize()函数的下面,而我自己是把already_running()函数写在了
     * daemonzie()函数里面.感觉书中的写法好一点,daemonize()函数只负责生成
     * 守护进程就可以了.守护进程生成后,最终的子进程从daemonize()函数返回,
     * 也是回到main()函数,并继续往下执行,能够执行到already_running()函数.
     * 虽然daemonize()函数中的父进程最好直接调用exit()函数终止进程,而不是
     * 通过return语句从daemonize()函数返回,从而执行到下面的already_running()
     * 函数,虽然父进程执行这个函数也没有什么影响.
     */
    // already_running();
    return 0;
}
