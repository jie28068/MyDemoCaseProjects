#include <stdio.h>

typedef unsigned int abc;
int main5()
{
	/*int *������һ���������ͣ�int*ָ�����ͣ�p���Ǳ�����
	������һ��ָ�����͵ı���������ָ��һ��int���ͱ����ĵ�ַ*/
	//int* p;
	//p = &a;//��a�ĵ�ַ��ֵ������p��pҲ��һ��������ֵ��һ���ڴ��ַ���
	//printf("%d\n", *p);//pָ����a�ĵ�ַ��*p����a��ֵ
	//char* p1 = &b;
	//printf("%c\n", *p1);//*p1ָ����b�ĵ�ַ��*p1����b��ֵ

	/*ָ��ļ�Ӵ�ֵ*/
	//int a = 0;
	//int b = 11;
	//int* p = &a;
	//*p = 100;
	//printf("a = %d, *p = %d\n", a, *p);
	//p = &b;
	//*p = 22;
	//printf("b = %d, *p = %d\n", b, *p);

	/*��ָ��*/
	//void* p = NULL;
	//int a = 10;
	//p = (void*)&a; //ָ�����ʱ�����ת��Ϊvoid *
	//ʹ��ָ�����ָ����ڴ�ʱ��ת��Ϊint *
	//*((int*)p) = 11;
	//printf("a = %d\n", a);

	/*�������������׸�Ԫ�صĵ�ַ��ͬ*/
	//int a[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };
	//printf("a = %p\n", a);
	//printf("a = %p\n", &a[0]);

	/*+1�ͻᵽ��һ��Ԫ�ص�ַ*/
	//int a[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };
	//int i = 0;
	//int n = sizeof(a) / sizeof(a[0]);
	//for (i = 0; i < n; i++)
	//{
	//	//printf("%d, ", a[i]);
	//	//printf("%d, ", *(a+i));
	//	printf("%d, ", *(&a[0] + i));
	//}

	/*ָ�������һ������*/
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

	/*ͨ������ָ���������*/
	//int a[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };
	//int i = 0;
	//int n = sizeof(a) / sizeof(a[0]);
	//int* p = a;
	//for (i = 0; i < n; i++)
	//{
	//	printf("%d, ", *p);
	//	p++;
	//}

	/*��ַ�ֽ�λ��*/
	//int a[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };
	//int* p2 = &a[2]; //��2��Ԫ�ص�ַ
	//int* p1 = &a[1]; //��1��Ԫ�ص�ַ
	//printf("p1 = %p, p2 = %p\n", p1, p2);
	//int n1 = p2 - p1; //n1 = 1  һ���ֽ��ĸ�λ
	//int n2 = (int)p2 - (int)p1; //n2 = 4      �ѵ�ַǿתΪInt
	//printf("n1 = %d, n2 = %d\n", n1, n2);

	/*ָ������*/
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
	
	/*����ָ��*/
	/*int a = 10;
	int* p = &a; //һ��ָ��
	*p = 100; //*p����a
	int** q = &p;
	//*q����p
	//**q����a
	int*** t = &q;
	//*t����q
	//**t����p
	//***t����a*/

	/*�ַ���ָ��*/
	//char str[] = "hello world";
	//char* p = str;//��ȡ�׵�ַ
	//*p = 'm';//���׵�ַ���ַ��滻Ϊm
	//p++;//��ַ��һ
	//*p = 'i';//����һ��ַ���ַ��滻Ϊi
	//printf("%s\n", str);//�����ֵӦΪmillo world
	//p = "mike jiang";//P����һ�����������ҾͿ��Զ�����ֵ
	//printf("%s\n", p);
	//char* q = "test";
	//printf("%s\n", q);
	//int* s = 123;
	//printf("%p\n",s);//����һ����������һ����ַûë����
	//printf("%d\n", s);

	/*���������������*/
	//abc abc = 1;
	//printf("%d", abc);


	system("pause");
	return 0;
}