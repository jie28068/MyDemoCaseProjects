#include <stdio.h>
/*
	��������������������жϸ����Ƿ�Ϊ������
	(��������ָ�����Ҷ�����ҵ����һ������12321)��
*/
int main49()
{
	char buf[10] = { 0 };
	char arr[10] = { 0 };
	int j = 0;
	int i = 0;
	fgets(buf, sizeof(buf), stdin);   
	for (int i = strlen(buf) - 1; i >= 0; i--)  //��������������棬Ȼ�����ٽ��������ֱȽ�
	{
			arr[j] = buf[i];
			j++;
	}
	for (i = 0; i < strlen(arr); i++)
	{
		printf("%c", arr[i]);
	}
	arr[i] = '\n';    //ע������ʱ�Ļس�����
	printf("\n");
	char* p1 = buf;
	char* p2 = arr;
	if (*p1 == *(p2+1))
	{
		printf("�ǻ�����\n");
	}
	else
	{
		printf("���ǻ�����\n");
	}
	system("pause");
	return 0;
}