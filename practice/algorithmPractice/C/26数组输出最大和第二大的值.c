#include <stdio.h>

int main26()
{
	int arr[6];
	printf("������������֣�\n");
	/*��������*/
	for (int i = 0; i < 5; i++)
	{
		scanf_s("%d", &arr[i]);
	}
	/*ð�����򣬴�С*/
	for (int j = 0; j < sizeof(arr) / sizeof(arr[0]) - 1; j++)
	{
		for (int k = 0; k < sizeof(arr) / sizeof(arr[0]) - 1 - j; k++)
		{
			if (arr[k] < arr[k + 1])
			{
				int temp = arr[k];
				arr[k] = arr[k + 1];
				arr[k + 1] = temp;
			}
		}
	}
	/*�����õݹ�*/
	for (int i = 0; i < 5; i++)
	{
		if (arr[i] == arr[i + 1])
		{	
			for (int j = i; j < 5-i; j++)
			{
				arr[j] = arr[j + 1];    //��ǰ��ֵ			
			}
		}
	}
	int b[6];
	printf("-------ȥ�غ�����ְ���С����--------------\n");
	for (int i = 0; i < 5; i++)
	{
		if (arr[i] != arr[i + 1]) //�������ĵط��������,ʵ���ϻ��Ǵ��ڵ�
		printf("%d\n", arr[i]);
	
	}
	printf("-----------------------------------------\n");
	printf("���������ǣ�%d\n", arr[0]);
	printf("�ڶ���������ǣ�%d\n", arr[1]);
	system("pause");
	return 0;
}