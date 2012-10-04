#include <stdio.h>
#include <sys/resource.h>

#define FMT "%10ld	"
#define doit(name)	pr_limits(#name, name)

static void pr_limits(char *name, int resource);

/* Every process has a set of resource limits, some of which can be queried
 * and changed by the getrlimit() and setrlimit() functions.
 * #include <sys/resource.h>
 * int getrlimit(int resource, struct rlimit *rlptr);
 * int setrlimit(int resource, const struct rlimit *rlptr);
 * 		Both return: 0 if OK, nonzero on error.
 *
 * Each call to these two functions specifies a single resource and a
 * pointer to the following structure:
 * struct rlimit {
 * 	rlim_t	rlim_cur;	// soft limit: current limit
 * 	rlim_t	rlim_max;	// hard limit: maximum value for rlim_cur
 * };
 *
 * An infinite limit is specified by the constant RLIM_INFINITY.
 *
 * The resource limits affect the calling process and are inherited by any
 * of its children.
 */
int main(void)
{
#ifdef RLIMIT_AS
	doit(RLIMIT_AS);
#endif
	doit(RLIMIT_CORE);
	doit(RLIMIT_CPU);
	doit(RLIMIT_DATA);
	doit(RLIMIT_FSIZE);
#ifdef RLIMIT_LOCKS
	doit(RLIMIT_LOCKS);
#endif
#ifdef RLIMIT_MEMLOCK
	doit(RLIMIT_MEMLOCK);
#endif
	doit(RLIMIT_NOFILE);
#ifdef RLIMIT_NPROC
	doit(RLIMIT_NPROC);
#endif
#ifdef RLIMIT_RSS
	doit(RLIMIT_RSS);
#endif
#ifdef RLIMIT_SBSIZE
	doit(RLIMIT_SBSIZE);
#endif
	doit(RLIMIT_STACK);
#ifdef RLIMIT_VMEM
	doit(RLIMIT_VMEM);
#endif

	return 0;
}

static void pr_limits(char *name, int resource)
{
	struct rlimit rlim;

	if (getrlimit(resource, &rlim) != 0) {
		printf("getrlimit %s, %d error\n", name, resource);
		return;
	}

	printf("%-14s	", name);

	if (rlim.rlim_cur == RLIM_INFINITY)
		printf("(infinite)	");
	else
		printf(FMT, rlim.rlim_cur);
	
	if (rlim.rlim_max == RLIM_INFINITY)
		printf("(infinite)	");
	else
		printf(FMT, rlim.rlim_max);

	putchar('\n');
}
