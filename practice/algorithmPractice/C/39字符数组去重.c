#include <stdio.h>
/*����ͬ���ַ��ÿգ�Ȼ��ȥ��*/
void func39(char* arr)
{
	char* p = arr;
	while (*p != '\0')
	{
		arr = p+1;
		while (*arr != '\0')
		{
			if (*p == *arr)
			{
				*arr = ' '; /*����ͬ�ĺ�һ���ַ��ÿ�*/
			}
			arr++;
		}
		p++;
	}
}
void To_empty39(char* Str)
{
	char* p = Str;
	while (*Str != '\0')
	{
		if (*Str != ' ')
		{
			*p++ = *Str;     
		}
		Str++;
	}
	*p = '\0'; 
}
int main39()
{
	char arr[30];
	fgets(arr, sizeof(arr), stdin);
	func39(arr);
	To_empty39(arr);
	printf("%s", arr);
	system("pause");
	return 0;
}