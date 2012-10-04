#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>	/* major(), minor() */

/* Every file system is known by its major and minor device numbers, which
 * are encoded in the primitive system data type dev_t. A disk drive often
 * contains several file systems. Each file system on the same disk drive
 * would usually have the same major number, but a different minor number.
 *
 * The st_dev value for every filename on a system is the device number of
 * the file system containing that filename and its corresponding i-node.
 * Only character special files and block special files have an st_rdev
 * value. This value contains the device number for the actual device.
 *
 * #include <sys/types.h>
 * int major(dev_t dev);		int minor(dev_t dev);
 * Given a device ID, major() and minor return, respectively, the major
 * and minor components.
 */
int main(int argc, char *argv[])
{
	int i;
	struct stat sb;

	for (i = 1; i < argc; ++i) {
		printf("%s: ", argv[i]);
		if (stat(argv[i], &sb) < 0) {
			printf("stat %s error: %s\n", argv[i], strerror(errno));
			continue;
		}

		printf("dev = %d/%d", major(sb.st_dev), minor(sb.st_dev));
		if (S_ISCHR(sb.st_mode) || S_ISBLK(sb.st_mode)) {
			printf(" (%s) rdev = %d/%d",
					(S_ISCHR(sb.st_mode)) ? "character" : "block",
					 major(sb.st_rdev), minor(sb.st_rdev));
		}
		printf("\n");
	}
	return 0;
}
