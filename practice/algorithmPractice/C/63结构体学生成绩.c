#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#if 0
struct stu1
{
	char* name;
	float scores[3];
};
int main63()
{
	struct stu1 s[3];
	for (int i = 0; i < 3; i++)
	{
		//���ٶѿռ�
		s[i].name = (char*)malloc(sizeof(char) * 21);
		if (NULL == s[i].name)
		{
			return;
		}
		printf("��������ѧ�� ����   �ɼ� ��\n");
		scanf("%s%f%f%f", s[i].name, &s[i].scores[0], &s[i].scores[1], &s[i].scores[2]);
	}
	//ð������
	for (int i = 0; i < 3 - 1; i++)
	{
		for (int j = 0; j < 3 - i - 1; j++)
		{
			int sum1 = s[j].scores[0] + s[j].scores[1] + s[j].scores[2];
			int sum2 = s[j + 1].scores[0] + s[j + 1].scores[1] + s[j + 1].scores[2];
			if (sum1 > sum2)
			{
				//�ṹ�彻��   �������г�Ա�б��е�����
				////��������
				//char temp[21] = { 0 };
				//strcpy(temp, s[j].name);
				//strcpy(s[j].name, s[j + 1].name);
				//strcpy(s[j + 1].name, temp);
				//�����ɼ�
				//for (int k = 0; k < 3; k++)
				//{
				//	float temp=s[j].scores[k];
				//	s[j].scores[k] = s[j + 1].scores[k];
				//	s[j + 1].scores[k] = temp;
				//}
				//�ṹ���������
				struct stu1 temp = s[j];
				s[j] = s[j + 1];
				s[j + 1] = temp;
			}
		}
	}
	printf("\n");
	
	for (int i = 0; i < 3; i++)
	{
		printf("������%s\n", s[i].name);
		printf("�ɼ�: %.1f   %.1f   %.1f\n", s[i].scores[0], s[i].scores[1], s[i].scores[2]);
		printf("�ܳɼ�: %.1f\n", (double)(s[i].scores[0]+s[i].scores[1]+s[i].scores[2]));
		printf("\n");
	}
	for (int i = 0; i < 3; i++)
	{
		free(s[i].name);
	}
	system("pause");
	return EXIT_SUCCESS;
}
#endif