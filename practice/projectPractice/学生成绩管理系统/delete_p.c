#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include "student.h"
void delete_p()
{
	FILE* dp = fopen("../text.txt", "r");
	FILE* p = fopen("../tex.txt", "w");
	FILE* p2 = fopen("../tex.txt", "r");
	int a = 0;
	int i = 0;
	int len = get_len(dp);
	look_p();
	printf("��������Ҫɾ��ѧ����ѧ��:");
	scanf("%d", &a);
	char d[1024] = { 0 };
	char b[1024] = { 0 };
	/*�뷨1���Ƚ�ÿ�е������ó������浽һ���ַ������У�
	�ٽ�ÿ�еĵ�һ���ַ�����������ݱȽϣ���ͬ�Ͱ���һ���ÿ�*/
	/*�뷨2���Ƚ�ÿ�еĵ�һ�����ݣ���������һ���ļ�������Ҫɾ���Ĳ�д�룬�����Ķ�д��
	Ȼ���ٰ������ʱ�ļ��滻��ԭ�е��ļ������У�*/
	stu s[100] = {0};
	for (i = 0; i < len; i++)
	{
		//s[i].id = malloc(sizeof(int));
		fgets(d, 30, dp);//���ж��������ַ�������
		sscanf(d, "%d %s %d %s\n", &s[i].id, s[i].name, &s[i].scores,s[i].sex);//��ʽ������
		//fscanf(dp, "%d %s %.1f", &s[i].id ,s[i].name ,&s[i].scores);		
		//printf("ѧ�ţ�%d ������%s �ɼ���%.1f\n", s[i].id, s[i].name, s[i].scores);	
		if (a == s[i].id)
		{
			continue;
		}
		else
		{
			fprintf(p, "%d %s %d %s\n", s[i].id, s[i].name, s[i].scores, s[i].sex);
		}
	}
	//fputs(d, pp);
	fseek(p, 0, SEEK_SET);//д�����һ���ļ�ʱ�����������棬Ҫ�����ŵ���ͷ��Ȼ����������
	int len2 = get_len(p2);
	FILE* pp = fopen("../text.txt", "w");
	for (i = 0; i < len2; i++)
	{
		fgets(b, 30, p2);
		sscanf(b, "%d %s %d %s\n", &s[i].id, s[i].name, &s[i].scores, s[i].sex);
		fprintf(pp, "%d %s %d %s\n", s[i].id, s[i].name, s[i].scores, s[i].sex);
	}
	printf("ɾ���ɹ�\n");
	fclose(p);
	fclose(dp);
	fclose(p2);
	fclose(pp);
	cmback();
	system("pause");
}