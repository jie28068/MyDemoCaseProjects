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
//	printf("������һ���ַ���\n");
//	fgets(ch, sizeof(ch), stdin);
//	func23(ch);
//	printf("%s\n",&ch[1]);
//	system("pause");
//	return 0;
//}
/*******�ݹ�ķ�ʽд�ַ������Ⱥ���*******/
//int my_strlen(char* str)
//{
//	if (*str == '\0')    //��������ַ�����û���ַ�
//		return 0;		//�ַ�������Ϊ0
//	else
//		return 1 + my_strlen(str + 1);	/*���õݹ飬ÿ�ݹ�һ��
//	���ȼ�1��ֱ����������'\0'ʱ�����ݹ�*/
//}
/*******�ݹ鷽ʽд�ַ�����ת*******/
void reverse_string(char* string)
{
	//int len = my_strlen(string);//����������ַ������Ⱥ���;
	int len = strlen(string);
	if (len <= 1)	//���ַ�������С�ڵ���1ʱ����ִ�У�
		return;
	else
	{
		char temp = string[0];	 //����һ���ַ���ֵ������temp��;
		string[0] = string[len - 1];//�����һ���ַ�������һ���ַ�;
		string[len - 1] = '\0';	//�����һ���ַ������ݸ�Ϊ'\0';
		reverse_string(string + 1);//�ݹ������һ�η�ת��
		string[len - 1] = temp;	//��temp������ǰ�����һ���ַ���
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


