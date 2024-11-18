#include <stdio.h>

//argc: 传参数的个数（包含可执行程序）
//argv：指针数组，指向输入的参数
int main6(int argc, char* argv[])
{

	//指针数组，它是数组，每个元素都是指针
	char* a[] = { "aaaaaaa", "bbbbbbbbbb", "ccccccc" };
	printf("%p\n", &a[0]);//数组的首地址与下面数组地址相同
	printf("%p\n", a);
	printf("%p\n", a[0]);//数组值（aaaaaaa）地址
	int b[] = { 1,2,3,4};
	printf("\n");
	printf("%p\n", b);
	printf("%d\n", b[0]);
	printf("%p\n", &b[0]);
	
	int i = 0;
	printf("argc = %d\n", argc);
	for (i = 0; i < argc; i++)
	{
		printf("%s\n", argv[i]);
	}
	system("pause");
	return 0;
}