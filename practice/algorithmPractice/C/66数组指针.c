#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Windows.h>
/*
   * User:SJ
   * Time:
   * Function:
   * Explain:
 */
typedef int(p_one)[5]; // ����һ             �൱�� typedef int [5] p_one ��p_oneΪ��ı���  
typedef int(*p_two)[5];//������
void main66()
{
	int arr[5] = {1,5,9,5,2};
	p_one* p1 = &arr;//����һ   �õ���������ĵ�ַ���ٶԸõ�ַ�����ã����������Ӧ����ֵ
	p_two p2 = &arr;//������    ֱ�Ӷ���Ϊһ��ָ�룬�õ������ַ
	for (int i = 0; i < sizeof(arr) / sizeof(arr[0]); i++)
	{
		printf(" %d\t", arr[i]);
	}
	printf("\n");
	for (int i = 0; i < sizeof(arr) / sizeof(arr[0]); i++)
	{
		printf(" %d\t", (*p1)[i]);//����һ  �����������
	}
	printf("\n");
	for (int i = 0; i < sizeof(arr) / sizeof(arr[0]); i++)
	{
		printf(" %d\t", *(*p2+i));//������  �Ƚ����õõ����������ٽ����õõ����������Ӧ��ֵ
	}
	int(*p_three)[3] = &arr;//������  ֱ�Ӷ���
	printf("\n");
	for (int i = 0; i < sizeof(arr) / sizeof(arr[0]); i++)
	{
		printf(" %d\t", *(*p_three+i));//������  �Ƚ����õõ����������ٽ����õõ����������Ӧ��ֵ
	}
	system("pause");
}
