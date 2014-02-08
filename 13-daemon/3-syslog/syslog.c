#include <syslog.h>

/* There are three ways to generate log messages:
 * 1. Kernel routines can call the log() function. These messages can be
 * read by any user process that openS and readS the /dev/klog device.
 * 2. Most user processes (daemons) call the syslog() function to generate
 * log messages. This causes the message to be sent to the UNIX domain
 * datagram socket /dev/log.
 * 3. A user process on this host, or on some other host that is connected
 * to this host by a TCP/IP network, can send log messages to UDP port 514.
 * Note that the syslog() function never generates these UDP datagrams:
 * they require explicit network programming by the process generating the
 * log message.
 * 这三种方式最终将数据传给syslogd守护进程,由syslogd来负责将数据写入文件,
 * 或者显示到控制台,或者再传给其他的主机(host).
 * #include <syslog.h>
 * void openlog(const char *ident, int option, int facility);
 * void syslog(int priority, const char *format, ...);
 * void closelog(void);
 * Calling openlog() is optional. If it's not called, the first time syslog
 * is called, openlog() is called automatically. Calling closelog() is also
 * optional--it just closes the descriptor that was being used to
 * communicate with the syslogd daemon.
 * openlog()函数的第一个参数可以指定一个嵌入到每条log开头的字符串.如果没有
 * 调用openlog()函数就直接调用syslog(),则syslog()默认会用所执行的命令名来
 * 嵌入到每条log开头,这一点是实际测试发现的,书中和man手册都没有提到这个规则
 * 关于 option, facility, priority 参数的具体取值要看一下man手册.
 *
 * int setlogmask(int mask);
 *              Returns: previous log priority mask value.
 * A process has a log priority mask that determines which calls to syslog
 * may be logged. All other calls will be ignored. Logging is enabled for
 * the priorities that have the corresponding bit set in mask. The initial
 * mask is such that logging is enabled for all priorities. If the mask
 * argument is 0, the current logmask is not modified.
 * The eight priorities are LOG_EMERG, LOG_ALERT, LOG_CRIT, LOG_ERR,
 * LOG_WARNING, LOG_NOTIE, LOG_INFO, and LOG_DEBUG. The bit corresponding to
 * a priority p is LOG_MASK(p).
 *
 * In addition to syslog(), many platforms provide a variant that handles
 * variable argument lists.
 * #include <syslog.h>
 * #include <stdarg.h>
 * void vsyslog(int priority, const char *format, va_list arg);
 */
int main(void)
{
    /* 这里虽然指定了log的优先级,但是查看/var/log/user.log文件,里面没有体现
     * 出来这些优先级,也就是log里面没有字段来表示log的优先级.
     */
    syslog(LOG_ERR, "zhong cheng juan shu");

    /* 调用openlog()指定log前缀为 "TIAN" */
    openlog("TIAN", LOG_CONS | LOG_PID, LOG_USER);
    syslog(LOG_ALERT, "tian xia you qing ren");

    /* 上面调用openlog()指定了LOG_PID选项,当时觉得下面调用closelog()后,这个
     * LOG_PID选项应该会失效,但是实际执行结果不是这样,后面的log语句也带上了
     * 进程ID,所以执行closelog()后,LOG_PID选项还是生效的.但奇怪的是,"TIAN"
     * 前缀却失效了,后面的log语句没有带有这个"TIAN"前缀.如果删掉closelog()
     * 语句,后面的log语句的确是带有"TIAN"前缀的,可能是因为LOG_PID选项是
     * openlog()函数独有的,所以closelog()函数不会让LOG_PID选项失效吧.
     */
    closelog();

    /* 要调用LOG_MASK()来将log优先级转换为对应的log mask.直接写为
     * setlogmask(LOG_ERR); 不会得到预期的结果.
     */
    setlogmask(LOG_MASK(LOG_ERR));

    /* 上面将log mask设置为了LOG_ERR,则下面LOG_CRIT优先级的log将不会被
     * 写入到log文件,且这句log会被忽略.即使后面将log mask设置为LOG_CRIT,
     * 这句log也不会被写入到log文件中.
     */
    syslog(LOG_CRIT, "how are you");

    /* LOG_ERR优先级的log能够写入到log文件 */
    syslog(LOG_ERR, "fine....");

    /* 将log mask设置为 LOG_CRIT后,下面的LOG_CRIT优先级log就能写入到log文件
     */
    setlogmask(LOG_MASK(LOG_CRIT));
    syslog(LOG_CRIT, "how are you again");

    return 0;
}
