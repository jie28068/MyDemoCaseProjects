#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
int main2()
{
	char a = 129; //127 + 2  +2后 1000 0001，这是负数补码，其原码为 1111 1111，结果为-127
	short int b = 129;
	long int c = 129;
	
	unsigned int e= 129;
	unsigned long int f = 129;
	unsigned char k = 129;
	//printf("%d\n", 100);
	//printf("%o\n", 100);
	//printf("%x\n", 100);
	//int aryy[5] = {1,2,3,4,5};
	//printf("%d\n", sizeof(aryy) / sizeof(aryy[0]));
	//printf("%d\n", aryy[5]);
	//printf("%d\n", sizeof (aryy));
	//printf("%d\n", SHRT_MAX);
	//printf("%d\n", SHRT_MIN);
	printf("%d\t%d\n", a,sizeof(a));
	printf("%d\t%d\n", b, sizeof(b));
	printf("%d\t%d\n", c, sizeof(c));
	long long int d = 129;
	printf("%lld\t%d\n", d, sizeof(d));
	printf("%d\t%d\n", e, sizeof(e));
	printf("%d\t%d\n", f, sizeof(f));
	printf("%d\t%d\n", k, sizeof(k));
	system("pause");
	return 0;
}