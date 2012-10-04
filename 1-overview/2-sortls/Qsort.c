#include <string.h>

static void swap(char *v[], int i, int j);

/* Qsort: sort v[left] ... v[right] into increasing order */
void Qsort(char *v[], int left, int right)
{
	int i, last;

	if (left >= right)
		return ;
	swap(v, left, (left+right)/2);
	last = left;
	for (i = left + 1; i <= right; ++i)
		if (strcmp(v[i], v[left]) < 0)		// 比较的是left
			swap(v, i, ++last);				// 加的是last
	swap(v, left, last);
	Qsort(v, left, last - 1);
	Qsort(v, last + 1, right);
}

static void swap(char *v[], int i, int j)
{
	char *temp;

	temp = v[i];
	v[i] = v[j];
	v[j] = temp;
}
