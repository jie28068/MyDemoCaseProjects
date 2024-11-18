#include <stdio.h>

int main8()
{
	    //'\b'           char
		//1066           int 
		//99.44          double
		//0XAA           int 
		//0x00EEABCDUL   long int
	char a;
	printf("请输入一个字母\n");
	scanf_s("%c", &a);
	printf("%d\n", a);
	int b;
	printf("请输入一个数字\n");
end:
	scanf_s("%d", &b);
	while (1)
	{
		if ((65 <= b && b <= 90) || ((b >=97 && b <=122)))
		{
			printf("%c\n", b);
			break;
		}
		else
		{
			printf("请输入有效数字\n");
			goto end;
		}

	}
	
	system("pause");
	return 0;
}