#include <stdio.h>

int main11()
{
	int arry[10] = {1,2,3,4,5,6,7,8,9,10};
	int* p =  arry;
	//p++;
	//*p = 100;
	//*(p + 1) = 100;
	//for (int i = 0; i < 5; i++)
	//{       
	//	//printf("%d\n", p[i]);    //��ָ���һ�����е�ָ�붼�������ƣ�ָ��ƫ�ƣ��ᷢ��Խ������
	//	//printf("%d\n", *(p + i));   
	//	//printf("%d\n", arry[i]);       

	//}
	p++;
	*p = 100;
	p = 100;
	p = &arry[9];
	*p = 100;
	p--;
	*p = 20;
	p = &arry[0];
	*p += 100;
	for (int i = 0; i < 10; i++)
	{       
	
		printf("%d\n", arry[i]);       

	}
	system("pause");
	return 0;
}