#include <stdio.h>
void func31(int n, char* buf)
{
	/*�ݹ�ʱҪ��if��else�Ĵ����Ŵ��*/
	if (n < 16)
	{
		printf("Ox");              //���ߵ�������
	}
	else
	{
		func31(n / 16, buf);        //Ȼ��������
	}
	printf("%c", buf[n % 16]);

}
int main31()
{	
	int a = 0;
	char buf[17] = "0123456789ABCDEF"; //����ʮ�������ֿ�
	printf("������һ��������\n");
	scanf_s("%d", &a);
	func31(a, buf);
	system("pause");
	return 0;
}

/*ѭ��ʵ��*/
//#include <stdio.h>
//#include <string.h>
//
//int main()
//{
//
//	int n,i=0,len=0;
//	char buf[17]="0123456789ABCDEF";//����ʮ�������ֿ�
//	char tem[100]={0};//�洢ת�����,���ȿ����޸�
//	char c;
//	printf("����һ��ʮ����������:");
//	scanf("%d",&n);
//	printf("ת����ʮ�����ƣ�");
//	while(1)
//	{
//		tem[i++] = buf[n%16];//i��¼�������ַ���������
//		n = n/16;
//		if(n == 0)
//		{
//			tem[i] = '\0';//��һ���ַ�������������������ʹ��strlen��������Ѵ��ַ�����
//			break;
//		}
//	}
//	len = strlen(tem);//���tem�����ַ��ĸ�����Ҳ����ֱ��ʹ�������i
//	printf("0x");
//	for(i=len-1;i>=0;i--)//���յ����tem������ַ�������
//	{
//		printf("%c",tem[i]);
//	}
//	printf("\n");
//	return 0;
//}
