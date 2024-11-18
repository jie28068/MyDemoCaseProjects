#include <stdio.h>
/*
	将一个数组中的值按逆序重新存放。例如，原来顺序为 a,b,c,d,e,f,g,
	现在顺序为g,f,e,d, c,b,a.(数组长度不限)

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
	printf("请输入一串字符:\n");
	fgets(buf, sizeof(buf), stdin);
	int len = strlen(buf);
	func51(buf, len);
	char* p = buf;
	printf("%s\n",p+1); //除掉回车问题
	system("pause");
	return 0;
}