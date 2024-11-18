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
	//	//printf("%d\n", p[i]);    //首指针加一，所有的指针都会往后移，指针偏移，会发生越界问题
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