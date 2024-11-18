#include <stdio.h>
void func31(int n, char* buf)
{
	/*递归时要把if，else的大括号打好*/
	if (n < 16)
	{
		printf("Ox");              //先走到最里面
	}
	else
	{
		func31(n / 16, buf);        //然后逐层输出
	}
	printf("%c", buf[n % 16]);

}
int main31()
{	
	int a = 0;
	char buf[17] = "0123456789ABCDEF"; //创建十六进制字库
	printf("请输入一个正整数\n");
	scanf_s("%d", &a);
	func31(a, buf);
	system("pause");
	return 0;
}

/*循环实现*/
//#include <stdio.h>
//#include <string.h>
//
//int main()
//{
//
//	int n,i=0,len=0;
//	char buf[17]="0123456789ABCDEF";//创建十六进制字库
//	char tem[100]={0};//存储转换结果,长度可以修改
//	char c;
//	printf("输入一个十进制正整数:");
//	scanf("%d",&n);
//	printf("转换成十六进制：");
//	while(1)
//	{
//		tem[i++] = buf[n%16];//i记录了整个字符串长度了
//		n = n/16;
//		if(n == 0)
//		{
//			tem[i] = '\0';//加一个字符串结束符，方便下面使用strlen函数获得已存字符个数
//			break;
//		}
//	}
//	len = strlen(tem);//获得tem里面字符的个数，也可以直接使用上面的i
//	printf("0x");
//	for(i=len-1;i>=0;i--)//按照倒序把tem里面的字符读出来
//	{
//		printf("%c",tem[i]);
//	}
//	printf("\n");
//	return 0;
//}
