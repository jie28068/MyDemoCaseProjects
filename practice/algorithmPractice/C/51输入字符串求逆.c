#include <stdio.h>
/*
	��һ�������е�ֵ���������´�š����磬ԭ��˳��Ϊ a,b,c,d,e,f,g,
	����˳��Ϊg,f,e,d, c,b,a.(���鳤�Ȳ���)

*/
void func51(char *buf,int len)
{
	char* p = buf;
	char* p1 = p + len - 1;
	while (p < p1)
	{
		char temp = *p;
		*p = *p1;
		*p1 = temp;
		p++;
		p1--;
	}

}
int main51()
{
	char buf[20] = { 0 };
	printf("������һ���ַ�:\n");
	fgets(buf, sizeof(buf), stdin);
	int len = strlen(buf);
	func51(buf, len);
	char* p = buf;
	printf("%s\n",p+1); //�����س�����
	system("pause");
	return 0;
}