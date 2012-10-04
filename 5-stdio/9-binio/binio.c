#include <stdio.h>

/* The following two functions are provided for binary I/O.
 * #include <stdio.h>
 * size_t fread(void *restrict ptr, size_t size, size_t nobj,
 * 				FILE *restrict fp);
 * size_t fwrite(const void *restrict ptr, size_t size, size_t nobj,
 * 				FILE *restrict fp);
 * Both return: number of objects read or written.
 * These functions have two common uses:
 * (1) Read or write a binary array. For example, to write elements 2
 *     through 5 of a floating-point array, we could write
 *     float data[10];
 *     if (fwrite(&data[2], sizeof(float), 4, fp) != 4)
 *     		err_sys("fwrite error");
 *     Here, we specify size as the size of each element of the array and
 *     nobj as the number of elements.
 * (2) Read or write a structure. For example, we could write
 *     struct {
 *     		short count;
 *			long total;
 *			char name[NAMESIZE];
 *	   } item;
 *	   if (fwrite(&item, sizeof(item), 1, fp) != 1)
 *	   		err_sys("fwrite error");
 *	   Here, we specify size as the size of structure and nobj as one.
 */
int main(void)
{
	int array[6] = {0x31, 0x32, 0x33, 0x34, 0x35, 0x36};
	int c;
	int *p;

	/* fwrite()函数操作的是 binary I/O,它会直接把第一个参数指向的内容打印
	 * 出来,而array[2]~array[5]对应的值分别是ASCII码中的'3','4','5','6',
	 * 所以下面把array[2]~array[5]中的内容写入stdout,打印出来的是3456.
	 * 如果array[2]~array[5]中保存的是3,4,5,6,则打印出来的会是乱码,因为3,4,
	 * 5,6对应的ASCII码不是可见字符.
	 */
	if (fwrite(&array[2], sizeof(int), 4, stdout) != 4)
		printf("fwrite array[2]~array[5] to stdout error:%m\n");
	printf("\n");

	c = 0x31323334;
	p = &c;

	/* 这里是要看取一个整型变量的地址,这个地址是指向该变量的最高字节,还是
	 * 指向变量的最低字节.从下面的输出结果可以看出,是指向变量的最低字节.
	 * (*p)打印出来的结果是4,而4是变量c的最低有效字节,IA32处理器是小端模式,
	 * 最低有效字节存储在低地址中,即 p 指向变量c的最低字节.
	 */
	printf("%p: %c\n", p, *p);

	/* 下面把整型变量c的二进制值输出到stdout中,从上面可以知道, &c 指向变量c
	 * 的最低字节,所以会先打印出4,然后依次是3,2,1,最终结果也正是4321,相符.
	 */
	fwrite(&c, sizeof(char), sizeof(int)/sizeof(char), stdout);
	printf("\n");

	/* 下面调用fread()函数从stdout中读取数据,每次读取sizeof(int)个长度,总共
	 * 读4个这样的对象,所以总共会读入16个字节,即从终端输入的是16个字符.实际
	 * 运行的时候,发现正是这样.例如,从终端输入的是:1234567891234567,则
	 * array[2]中存的是0x34333231. &array[2]指向array[2]的最低字节,先输入的
	 * 1被转换为ASCII码0x31存入array[2]的最低字节中,后面以此类推,所以,最后
	 * array[2]中存的是0x34333231.
	 */
	if (fread(&array[2], sizeof(int), 4, stdin) != 4)
		if (ferror(stdin))
			printf("fread array[2]~array[5] from stdin error:%m\n");
	if (fwrite(&array[2], sizeof(int), 4, stdout) != 4)
		printf("fwrite array[2]~array[5] to stdout error:%m\n");

	/* 打印 array[2] 的二进制值,证明上面的说法 */
	printf("array[2]中存储的值是:%x\n", array[2]);

	return 0;
}
