//#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <Windows.h>
#include "student.h"
/*
   * User:SJ
   * Time:2021/8/17
   * Function:
   * Explain:
 */
void add_p()
{
	FILE* p = fopen("../text.txt", "a+");
	FILE* fp = fopen("../text.txt", "r");
	if (NULL == fp)
	{
		return;
	}
	int Max=get_len(fp);//��ȡ�ļ�����
	int b[1024] = {0};
	char d[1024] = {0};
	int i = 0;
	int a = 0;
	if (NULL == p)
	{
		return;
	}
	stu *s=(stu*)malloc(sizeof(stu));
	if (NULL == s)
	{
		return;
	}
star_p:
	printf("������ѧ��  ����  �ɼ� �Ա�XY/X��\n");
	//scanf("%d %s %.1f", &(s->id), s->name, &(s->scores));
	scanf("%d %s %d %s", &(s->id), s->name, &(s->scores) ,s->sex);
	while (fgets(d, 30, fp))//���е�ȡ��txt�е�����
	{	//d�ĵ�ַ����һ����,������ָ������
		b[i++] =get_once(d);//��ÿһ�еĵ�һ���ַ�ȡ��	
	}
	//int n = 0;
	//sscanf(b, "%d", &n);//���ַ�����ת��Ϊint����
	/*��ֹ���ظ���ѧ��*/
	for (i = 0; i < Max; i++)
	{
		if ((s->id) == b[i])
		{
			printf("��ѧ���Ѵ��ڣ����������룡\n");
			goto star_p;
		}
	}
	fprintf(p, "%d %s %d %s \n", s->id, s->name, s->scores, s->sex);
	printf("¼��ɹ�\n");
	free(s);
	fclose(p);
	fclose(fp);
	cmback();
	//system("pause");
}
