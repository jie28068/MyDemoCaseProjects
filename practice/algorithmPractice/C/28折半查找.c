#include <stdio.h>
/*�۰����Ҫ������ֻ���ҳ�һ��*/
int main28()
{	
	int a[] = { 13,14,78,90,189,355,189 };
	int key;
	/*0*/
	int min=0;
	/*�м�ֵ*/
	int mid = 0;
	/*���鳤��*/
	int max = sizeof(a) / sizeof(a[0]);
	scanf_s("%d", &key);
	int flag = 0;
	while (min <= max)
	{
		 mid = (min + max) / 2;
		 /*�����ֵ�����м�ֵʱ�����¶�min��ֵ*/
		if (key > a[mid])
		{
			min = mid + 1;
		}
		/*�����ֵС���м�ֵʱ�����¶�max��ֵ*/
		else if (key < a[mid])
		{
			max = mid - 1;
		}
		else
		{
			printf("%d,%d", a[mid],mid);
			flag = 1;
			break;
		}
	}
	if (0 == flag)
	{
		printf("û���������\n");
	}
	system("pause");
	return 0;
}