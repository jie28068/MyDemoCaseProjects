#include <stdio.h>

int main27()
{	
	int a[] = { 1,58,54,88,99,5,8,12,64,54,2,100};
	int b = 0;
	for (int i = 0; i < 12; i++)
	{
			printf("%d\n", a[i]);
	}
	printf("������һ������\n");
	scanf_s("%d", &b);
	int len = sizeof(a) / sizeof(a[0]);
	int temp = len;
	int* p = a;
	/*�趨һ����־λ�������ж������������Ƿ����������*/
	int flag = 0;         
	
	/*ָ��ʵ�ַ�ʽ*/
	//while (len)
	//{	
	//	if (*p == b)
	//	{				
	//		printf("��Ӧ�������±�Ϊ��%d\n", (temp - len));
	//		flag = 1;
	//	}
	//	len--;
	//	p++;
	//}
	/*����ʵ�ַ�ʽ*/
	for (int i = 0; i < len; i++)
	{
		if (a[i] == b)
		{
			printf("��Ӧ�������±�Ϊ��%d\n", i);
			flag = 1;
		}
	}
	if (!flag)
	{
		printf("��������û�и�����\n");
	}
	system("pause");
	return 0;
}