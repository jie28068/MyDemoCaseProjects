#include <stdio.h>
#include <Windows.h>
/*ѡ������ÿ��ѭ��ѡ����С���������ַŵ���ǰ��*/
int main30()
{
	int a[] = { 1,25,6,99,87,50,10 };
	for (int i = 0; i < 7; i++)
	{
		int index = i;           //��Ϊһ�����λ��ȥ��¼����ѭ����С������λ��
		for (int j = i + 1; j < 7; j++)//ѭ���ҳ�������С������
		{
			if (a[index] > a[j])
				index = j;//�ҵ���С��λ��
		}
		int temp = a[index];//�ڱ���ѭ���ҳ���������Сֵ����ѭ����ǰ���ֵ�����滻
		a[index] = a[i];
		a[i] = temp;
	}
	for (int i = 0; i < 7; i++)
	{
		printf("%d\t", a[i]);
	}

	system("pause");
	return 0;
}