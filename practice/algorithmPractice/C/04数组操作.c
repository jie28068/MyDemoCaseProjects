#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main4()
{
	int arry[5];
	printf("%p\n", arry);
	printf("%p\n", &arry[0]);
	printf("%p\n", &arry[4]);
	char a[] = { 'a','b','c' };
	printf("a=%s\n", a);//没有以‘\0’结尾，所以会乱码
	char b[] = { 'a','b','c','\0' };
	printf("b=%s\n", b);
	char buf7[50] = { '1', 'a', 'b', '\0', '7' };
	printf("buf7 = %s\n", buf7);
	//'\ddd'八进制字义字符，'\xdd'十六进制转移字符
	// \012相当于\n
	char str[] = "\012abc";
	printf("str == %s\n", str);

	time_t tm = time(NULL);//得到系统时间
	srand((unsigned int)tm);//随机种子只需要设置一次即可
	//等价于srand((unsigned int)time(UNLL));
	int r = rand();
	printf("r = %d\n", r);

	system("pause");
	return 0;
}