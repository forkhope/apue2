#include "apue.h"
#include <errno.h>
#include <limits.h>

/* If the constant PATH_MAX is defined in <limits.h>, then we're all set.
 * If it's not, we need to call pathconf().
 */
#ifdef PATH_MAX
static int pathmax = PATH_MAX;
#else
static int pathmax = 0;
#endif

#define SUSV3 201112L

static long posix_version = 0;

/* If PATH_MAX is indeterminate, no guarantee this is adequate */
#define PATH_MAX_GUESS	1024

/* Dynamically allocate space for a pathname */
char *path_alloc(int *sizep)  /* alse return allocated size, if nonnull */
{
	char *ptr;
	int size;

	if (posix_version == 0)
		posix_version = sysconf(_SC_VERSION);

	if (pathmax == 0) { 	/* first time through */
		errno = 0;
		/* The vaule returned by pathconf() is the maximum size of a 
		 * relative pathname when the first argument is the working
		 * directory, so we specify the root as the first argument and add
		 * 1 to the result.
		 * long pathconf(char *path, int name);
		 * _PC_PATH_MAX: returns the maximum length of a relative pathname
		 * when path is the current working directory.
		 */
		if ((pathmax = pathconf("/", _PC_PATH_MAX)) < 0) {
			/* If the system does not have a limit for the requested
			 * resource, -1 is returned, and errno is unchanged. If there
			 * is an error, -1 is returned, and errno is set to reflect
			 * the nature of the error
			 */
			if (errno == 0)	// 此时,errno没有变,对应上面说的第一种情况
				pathmax = PATH_MAX_GUESS; 	/* it's indeterminate */
			else
				err_sys("pathconf error for _PC_PATH_MAX");
		}
		else
			++pathmax;		/* add one since it's relative to root */
	}
	if (posix_version < SUSV3)
		size = pathmax + 1;
	else
		size = pathmax;

	if ((ptr = malloc(size)) == NULL)
		err_sys("malloc error for pathname");

	if (sizep != NULL)
		*sizep = size;
	return ptr;
}
