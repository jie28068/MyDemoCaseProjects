#include <stdio.h>
/*
	��д������Ϸ���������ĳ���������Ӽ����Ϸ����������ݽ��в²⡣��
	��δ���У�������ʾ���������߹�С;������У�������µĴ������������
	��10�Ρ�
*/
int main50()
{
	int a = 0;
	int count = 0;
	while (56 != a)
	{
		scanf_s("%d", &a);
		if (count <= 10)
		{
			if (a > 56)
			{
				printf("����");
				count++;
			}
			else if (a < 56)
			{
				printf("С��");
				count++;
			}
			else
			{
				count++;
				break;
			}
		}
		else
		{
			printf("�������꣡");
		}
	}
	printf("56!!\n������%d",count);
	system("pause");
	return 0;
}