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
#if 0
typedef struct opt
{
	char* name;
	int stu;
}p;
//�����ڴ�
p** change()
{	/*�������ޣ��ڶѿռ俪�ٽṹ�����ָ�룬�����Žṹ��һ��ָ�룬�ṹ��һ��ָ��ָ���˿��ٶѿռ�
	��ŵ�char *name�� int stu��char* name�п����˶ѿռ�ָ���˴��name��ֵ */
	p** temp = malloc(sizeof(p*)*3);
	if (NULL == temp)
		return;
	for (int i = 0; i < 3; i++)
	{	
		temp[i] = malloc(sizeof(p));
		if (NULL == temp[i])
			return;
		temp[i]->name = malloc(sizeof(char) * 10);
		if (NULL == temp[i]->name)
			return;
		temp[i]->stu = i + 1;
		sprintf(temp[i]->name, "Name_%d", i + 1);
	}
	return temp;
}

//�ͷ��ڴ�
void fere(p**temp)
{	
	/*�ͷŶѿռ�ʱ�������浽���ͷ�*/
	if (NULL == temp)
		return;
	for (int i = 0; i < 3; i++)
	{	
		if (NULL == temp[i])
			continue;
		if (temp[i]->name != NULL)
		{
			free(temp[i]->name);
			temp[i]->name = NULL;
		}
		free(temp[i]);
		temp[i] = NULL;
	}
	free(temp);
	temp = NULL;
	printf("�ͷųɹ���\n");
}
//��ӡ
void print(p**temp)
{
	for (int i = 0; i < 3; i++)
	{
		printf("������%s ����:%d\n ", temp[i]->name, temp[i]->stu);
	}

}

int main67()
{
	p** P_one= change();
	print(P_one);
	fere(P_one);
	system("pause");
	return 0;
}
#endif