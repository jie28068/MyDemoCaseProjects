#include <stdio.h>
/*
	���⣺A˵����ǰ��λ��ͬ��B˵����λ��ͬ������ǰ��λ��ͬ��C˵���ƺŸպ���ĳλ����ƽ��
	      ������ƺš�
	�Ż������ñ�־λ�����ҵ����ֺ��˳���û�б�Ҫ�ڴ�ѭ����ȥ��
*/
int main07()
{	
	int k,i,j,temp;
	int falg = 0;
	for (i = 0; i < 10; i++)
	{
		if (falg) 
			break;
		for ( j = 0; j < 10; j++)
		{
			if (falg)
				break;
			if (i != j)
			{
				k = 1000 * i + 100 * i + 10 * j + j;
				for (temp = 31; temp < 99; temp++)
					if (temp * temp == k)
					{
						printf("���ƺ�Ϊ%d\n", k);
						falg = 1;
						break;           //�˳����ڲ�ѭ��
					}
					
			}
		}
	}
	system("pause");
	return 0;
}