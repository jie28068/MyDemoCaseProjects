#include <stdio.h>
/*
	��дһ����������һ��3λ������������Ȼ���������Ӧ������
	��:123�������321��
*/
int main46()
{
	char buf[4] = { 0 };
	fgets(buf, sizeof(buf), stdin);
	for (int i = strlen(buf)-1; i >= 0; i--)
	{
		printf("%c", buf[i]);
	}
	printf("\n");
	system("pause");
	return 0;
}