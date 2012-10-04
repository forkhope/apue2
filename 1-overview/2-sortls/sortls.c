#include "apue.h"
#include <dirent.h>

// 一个目录中,包含的最大文件数目,第1000个以外的文件将不被打印
#define MAXFILE	1000

/* Prints the name of every file in a directory, and nothing else.
 * Note that the directory listing is in alphabetical order.
 */
int main(int argc, char *argv[])
{
	DIR				*dp;
	struct dirent	*dirp;
	int 			nfiles, i;
	char 			*files[MAXFILE];

	if (argc != 2)
		err_quit("Usage: sortls directory");

	if ((dp = opendir(argv[1])) == NULL)
		err_sys("sortls: can't open %s", argv[1]);

	nfiles = 0;
	while ((dirp = readdir(dp)) != NULL && nfiles < MAXFILE)
		files[nfiles++] = strdup(dirp->d_name);

	if (nfiles >= MAXFILE)
		fprintf(stderr, "sortls: too many files, just list 1000 files\n");
	Qsort(files, 0, nfiles - 1);
	for (i = 0; i < nfiles; ++i) {
		printf("%s\n", files[i]);
		free(files[i]);
	}

	closedir(dp);
	exit(0);
}
