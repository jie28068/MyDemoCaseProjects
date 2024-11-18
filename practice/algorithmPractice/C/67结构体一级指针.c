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
//分配内存
p** change()
{	/*三层套娃，在堆空间开辟结构体二级指针，里面存放结构体一级指针，结构体一级指针指向了开辟堆空间
	存放的char *name和 int stu，char* name有开辟了堆空间指向了存放name的值 */
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

//释放内存
void fere(p**temp)
{	
	/*释放堆空间时，从里面到外释放*/
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
	printf("释放成功！\n");
}
//打印
void print(p**temp)
{
	for (int i = 0; i < 3; i++)
	{
		printf("姓名：%s 年龄:%d\n ", temp[i]->name, temp[i]->stu);
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