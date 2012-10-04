#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/times.h>
#include <sys/wait.h>

static void pr_exit(int status);
static void pr_times(clock_t, struct tms *, struct tms *);
static void do_cmd(char *);

/* In Section 1.10, we described three times that we can measure: wall clock
 * time, user CPU time, and system CPU time. Any process can call the times
 * function to obtain these values for itself and any terminated children.
 * #include <sys/times.h>
 * clock_t times(struct tms *buf);
 * 		Returns: elapsed wall clock time in clock ticks if OK, -1 on error
 * This function fills in the tms structure pointed to by buf:
 * struct tms {
 * 	clock_t tms_utime;		// user CPU time
 * 	clock_t tms_stime;		// system CPU time
 * 	clock_t tms_cutime;		// user CPU time, terminated children
 * 	clock_t tms_sutime;		// system CPU time, terminated children
 * };
 *
 * Note that the structure does not contain any measurement for the wall
 * clock time. Instead, the function returns the wall clock time as the
 * value of the function, each time it's called. We can't user its absolute
 * value; instead, we use its relative value. For example, we call times()
 * and save the return value. As some later time, we call times() again and
 * subtract the earlier return value from the new return value. The
 * difference is the wall clock time.
 *
 * The two structure fields for child processes contain values only for
 * children that we have waited for with wait(), waitid(), or waitpid().
 */
int main(int argc, char *argv[])
{
	int i;

	setbuf(stdout, NULL);
	/* 注意,下面 i 的初值不能写为 i = 0,否则会陷入死循环,因为当i等于0时,
	 * argv[i]就是argv[0],而argv[0]正是当前正在运行的程序,下面再调用do_cmd
	 * 执行一次argv[0]程序,从而陷入死循环.
	 */
	for (i = 1; i < argc; ++i)
		do_cmd(argv[i]);	/* once for each command-line arg */

	return 0;
}

/* execute and time the "cmd" */
static void do_cmd(char *cmd)
{
	clock_t start, end;
	struct tms tmsstart, tmsend;
	int status;

	printf("comand: %s\n", cmd);

	if ((start = times(&tmsstart)) == -1)	/* starting values  */
		return;
	if ((status = system(cmd)) < 0)	/* execute command */
		return;
	if ((end = times(&tmsend)) == -1)	/* ending values */
		return;

	pr_times(end-start, &tmsstart, &tmsend);
	pr_exit(status);
}

static void pr_times(clock_t real, struct tms *tmsstart, struct tms *tmsend)
{
	static long clktck = 0;

	/* All the clock_t values returned by times() function are converted to
	 * seconds using the number of clock ticks per second--the
	 * _SC_CLK_TCK value returned by sysconf (Section 2.5.4).
	 */
	if (clktck == 0)	/* fetch clock ticks per second first time */
		if ((clktck = sysconf(_SC_CLK_TCK)) < 0)
			return;
	printf(" real: %7.2f\n", real / (double)clktck);
	printf(" user: %7.2f\n", 
			(tmsend->tms_utime-tmsstart->tms_utime) / (double)clktck);
	printf(" sys: %7.2f\n",
			(tmsend->tms_stime-tmsstart->tms_stime) / (double)clktck);
	printf(" children user: %7.2f\n",
			(tmsend->tms_cutime-tmsstart->tms_cutime) / (double)clktck);
	printf("children sys: %7.2f\n",
			(tmsend->tms_cstime-tmsstart->tms_cstime) / (double)clktck);
}

static void pr_exit(int status)
{
	if (WIFEXITED(status))
		printf("normal termination, exit statusn = %d\n",
				WEXITSTATUS(status));
	else if (WIFSIGNALED(status))
		printf("abnormal termination, signal number = %d%s\n",
				WTERMSIG(status),
#ifdef WCOREDUMP
				WCOREDUMP(status) ? "(core file geneated)" : "");
#else
				"");
#endif
	else if (WIFSTOPPED(status))
		printf("child stopped, signal number = %d\n",
				WSTOPSIG(status));
}
