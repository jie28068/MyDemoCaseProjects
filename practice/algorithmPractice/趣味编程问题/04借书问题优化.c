#include <stdio.h>
/*
	���⣺С�����屾�飬���A��B��C�����ˣ�ÿ����ÿ��ֻ�ܽ�һ�����ж����ֽ跨
	��������ǰA��BҪ��������ͬʱ����ôC��û�б�Ҫ����ѭ�������Ծ��������Ч��
	     ����������a!=b��c��ѭ���С�
*/
int main04()
{
	int count = 0;
	for (int a = 1; a <= 5; a++)
	{
		for (int b = 1; b <= 5; b++)
		{
			for (int c = 1; c <= 5&&a!=b; c++)
			{
				if ( a != c && c != b)
				{
					//if(a + b + c == 5)
					printf("A:%d\tB:%d\tC:%d\t", a, b, c);
					count++;
					if (count % 5 == 0)
					{
						printf("\n");
					}
				}
			}
		}

	}
	printf("��%d�ַ���\n", count);
	system("pause");
	return 0;
}