#include <stdio.h>

typedef unsigned int abc;
int main5()
{
	/*int *代表是一种数据类型，int*指针类型，p才是变量名
	定义了一个指针类型的变量，可以指向一个int类型变量的地址*/
	//int* p;
	//p = &a;//将a的地址赋值给变量p，p也是一个变量，值是一个内存地址编号
	//printf("%d\n", *p);//p指向了a的地址，*p就是a的值
	//char* p1 = &b;
	//printf("%c\n", *p1);//*p1指向了b的地址，*p1就是b的值

	/*指针的间接传值*/
	//int a = 0;
	//int b = 11;
	//int* p = &a;
	//*p = 100;
	//printf("a = %d, *p = %d\n", a, *p);
	//p = &b;
	//*p = 22;
	//printf("b = %d, *p = %d\n", b, *p);

	/*空指针*/
	//void* p = NULL;
	//int a = 10;
	//p = (void*)&a; //指向变量时，最好转换为void *
	//使用指针变量指向的内存时，转换为int *
	//*((int*)p) = 11;
	//printf("a = %d\n", a);

	/*数组名与数组首个元素的地址相同*/
	//int a[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };
	//printf("a = %p\n", a);
	//printf("a = %p\n", &a[0]);

	/*+1就会到下一个元素地址*/
	//int a[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };
	//int i = 0;
	//int n = sizeof(a) / sizeof(a[0]);
	//for (i = 0; i < n; i++)
	//{
	//	//printf("%d, ", a[i]);
	//	//printf("%d, ", *(a+i));
	//	printf("%d, ", *(&a[0] + i));
	//}

	/*指针可以做一个数组*/
	//int a[] = { 1, 2, 3, 4, 5, 6, 7, 55, 9 };
	//int i = 0;
	//int n = sizeof(a) / sizeof(a[0]);
	//int* p = a; 
	//for (i = 0; i < n; i++)
	//{
	//	p[i] =a[i];
	//}
	////printf("%d\n",p[1]);
	//for (i = 0; i < n; i++)
	//{
	//	printf("%d, ", *(p + i));
	//}

	/*通过操作指针操作数组*/
	//int a[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };
	//int i = 0;
	//int n = sizeof(a) / sizeof(a[0]);
	//int* p = a;
	//for (i = 0; i < n; i++)
	//{
	//	printf("%d, ", *p);
	//	p++;
	//}

	/*地址字节位数*/
	//int a[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };
	//int* p2 = &a[2]; //第2个元素地址
	//int* p1 = &a[1]; //第1个元素地址
	//printf("p1 = %p, p2 = %p\n", p1, p2);
	//int n1 = p2 - p1; //n1 = 1  一个字节四个位
	//int n2 = (int)p2 - (int)p1; //n2 = 4      把地址强转为Int
	//printf("n1 = %d, n2 = %d\n", n1, n2);

	/*指针数组*/
	//int* p[3];
	//int a = 1;
	//int b = 2;
	//int c = 3;
	//int i = 0;
	//p[0] = &a;
	//p[1] = &b;
	//p[2] = &c;
	//for (i = 0; i < sizeof(p) / sizeof(p[0]); i++)
	//{
	//	printf("%d, \n", *(p[i]));
	//	printf("%p",p[i]);
	//}
	
	/*二级指针*/
	/*int a = 10;
	int* p = &a; //一级指针
	*p = 100; //*p就是a
	int** q = &p;
	//*q就是p
	//**q就是a
	int*** t = &q;
	//*t就是q
	//**t就是p
	//***t就是a*/

	/*字符串指针*/
	//char str[] = "hello world";
	//char* p = str;//获取首地址
	//*p = 'm';//将首地址的字符替换为m
	//p++;//地址加一
	//*p = 'i';//将下一地址的字符替换为i
	//printf("%s\n", str);//输出的值应为millo world
	//p = "mike jiang";//P就是一个变量，那我就可以对他赋值
	//printf("%s\n", p);
	//char* q = "test";
	//printf("%s\n", q);
	//int* s = 123;
	//printf("%p\n",s);//我是一个变量，有一个地址没毛病吧
	//printf("%d\n", s);

	/*给数据类型起别名*/
	//abc abc = 1;
	//printf("%d", abc);


	system("pause");
	return 0;
}