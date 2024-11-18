#include<stdio.h>
//
//void func23(char* string)
//{
//	int len = strlen(string);
//	if (len <= 1)	
//		return;
//	else
//	{
//		char temp = string[0];	 
//		string[0] = string[len - 1];
//		string[len - 1] = '\0';	
//		func23(string + 1);
//		string[len - 1] = temp;	
//	}
//}
//
//int main()
//{
//	char ch[100];
//	printf("请输入一串字符：\n");
//	fgets(ch, sizeof(ch), stdin);
//	func23(ch);
//	printf("%s\n",&ch[1]);
//	system("pause");
//	return 0;
//}
/*******递归的方式写字符串长度函数*******/
//int my_strlen(char* str)
//{
//	if (*str == '\0')    //当传入的字符串中没有字符
//		return 0;		//字符串长度为0
//	else
//		return 1 + my_strlen(str + 1);	/*运用递归，每递归一次
//	长度加1，直到遍历到的'\0'时结束递归*/
//}
/*******递归方式写字符串反转*******/
void reverse_string(char* string)
{
	//int len = my_strlen(string);//调用上面的字符串长度函数;
	int len = strlen(string);
	if (len <= 1)	//当字符串长度小于等于1时，不执行；
		return;
	else
	{
		char temp = string[0];	 //将第一个字符的值保存在temp中;
		string[0] = string[len - 1];//将最后一个字符赋给第一个字符;
		string[len - 1] = '\0';	//将最后一个字符的内容赋为'\0';
		reverse_string(string + 1);//递归调用下一次反转；
		string[len - 1] = temp;	//将temp赋给当前的最后一个字符；
	}

}

int main23()
{
	char ch[100];
	fgets(ch, sizeof(ch), stdin);
	reverse_string(ch);
	printf("%s\n",&ch[1]);
	system("pause");
	return 0;
}


