//#include<stdio.h>
//#include<time.h>	//ʱ�亯�� 
//#include<stdlib.h>	//���������Ҫ�Ŀ� 
//#include <conio.h>  //char ch = _getch(); //��Ҫͷ�ļ�#include <conio.h>*/
////����  SIZE���ɱ� 
//#define SIZE 4
////�ո�����
//int num = 16;
////ʵ�ʵ�����
//int a[SIZE][4] = { 0 };
////������� 
//int b[10] = { 2,2,2,2,2,2,4,2,2,2 };
//int x1 = 0, y1 = 0, num1 = 0; //ȫ�ֱ����������������ĸ�ֵ
//void create(int l)
//{
//	int i, j, count;
//	int k, n, m;
//	//���������
//	srand(time(NULL));
//	for (m = 0; m < l; m++)
//	{
//		count = 0;
//		n = rand() % num + 1;//�����������λ��
//		k = rand() % 10;//ȡ������b[]�е�һ������
//		for (i = 0; i < SIZE; i++)
//		{
//			for (j = 0; j < SIZE; j++)
//			{
//				if (a[i][j] == 0)//Ϊ��ʱ����λ�Ż�Ӽӣ����Աز����ܺ���һ��λ�÷�����ͻ
//				{
//					count++;
//				}
//				if (count == n)//�����������λ��
//				{
//					a[i][j] = b[k];
//					num--;      
//					goto loop;  //��������ѭ��
//				}
//			}
//		}
//	loop:
//		{}
//	}
//}
//void interface_2048(void)
//{
//	//�о���͸ĳ�  const char *s=" +����-+"; 
//	char* s = " +����-+";
//	int i, j, k = 0, n;
//	for (i = 0; i < SIZE; i++)
//	{
//		for (j = 0; j < SIZE; j++)
//		{
//			printf("%s", s);
//		}
//		putchar('\n');
//		for (j = 0; j < 4; j++)
//		{
//			if (a[i][j] == 0)
//			{
//				printf(" |     |");
//				continue;               //��������ѭ��
//			}
//			n = 1;
//			printf(" |");
//			k = a[i][j];
//			do
//			{ 
//				n++;                  //������˼���
//			} while ((k /= 10) > 9); 
//			if (n < 3)                            //Ϊ��ʹ���־���
//			{
//				printf("  %-3d|", a[i][j]);
//			}
//			else if (n < 5)
//			{
//				printf(" %-4d|", a[i][j]);
//			}
//			else
//			{
//				printf("%5d|", a[i][j]);
//			}
//		}
//		putchar('\n');
//		for (j = 0; j < SIZE; j++)
//		{
//			printf("%s", s);
//		}
//		putchar('\n');
//	}
//	printf("��ѡ���ƶ������ϡ��¡����ң�w��s��a��d��\n����>");
//}
//void left()
//{
//	/*����*/
//	int i, j, z, tmp;
//	for (i = 0; i < 4; i++)//����������Ϊ��
//		for (j = 0; j < 4; j++)
//			if (a[i][j] == 0)   
//				for (z = j + 1; z < 4; z++)  //����������ֵ��ұ�һ������
//					if (a[i][z] > 0)   //���������ֵ��ұߵ����ֲ�Ϊ�㣬�ͽ�������ֺ���ߵ�����н���
//					{
//						tmp = a[i][j];  //��ʵ���Ƶ�Ч��
//						a[i][j] = a[i][z];
//						a[i][z] = tmp;
//						break;
//					}
//	for (i = 0; i < 4; i++)//�������������ͬ��
//		for (j = 0; j < 4; j++)
//			if (a[i][j] > 0)      
//				for (z = j + 1; z < 4; z++)
//					if (a[i][z] > 0)       //ͨ������if����ж�
//						if (a[i][z] == a[i][j])  //����������ߵ�������ͬ
//						{
//							a[i][j] *= 2;    //�����*2���ұ�����
//							a[i][z] = 0;
//						}
//						else
//							break;
//					else
//						break;
//			else
//				break;
//}
//void right()
//{
//	int i, j, z, tmp;
//	for (i = 0; i < 4; i++)
//		for (j = 3; j >= 0; j--)
//			if (a[i][j] == 0)
//				for (z = j - 1; z >= 0; z--)
//					if (a[i][z] > 0)
//					{
//						tmp = a[i][j];
//						a[i][j] = a[i][z];
//						a[i][z] = tmp;
//						break;
//					}
//	for (i = 0; i < 4; i++)
//		for (j = 3; j >= 0; j--)
//			if (a[i][j] > 0)
//				for (z = j - 1; z >= 0; z--)
//					if (a[i][z] > 0)
//						if (a[i][z] == a[i][j])
//						{
//							a[i][j] *= 2;
//							a[i][z] = 0;
//						}
//						else
//							break;
//					else
//						break;
//			else
//				break;
//}
//void up()
//{
//	int i, j, z, tmp;
//	for (i = 0; i < 4; i++)
//		for (j = 0; j < 4; j++)
//			if (a[j][i] == 0)
//				for (z = j + 1; z < 4; z++)
//					if (a[z][i] > 0)
//					{
//						tmp = a[j][i];
//						a[j][i] = a[z][i];
//						a[z][i] = tmp;
//						break;
//					}
//	for (i = 0; i < 4; i++)
//		for (j = 0; j < 4; j++)
//			if (a[j][i] > 0)
//				for (z = j + 1; z < 4; z++)
//					if (a[z][i] > 0)
//						if (a[z][i] == a[j][i])
//						{
//							a[j][i] *= 2;
//							a[z][i] = 0;
//						}
//						else
//							break;
//					else
//						break;
//			else
//				break;
//}
//void down()
//{
//	int i, j, z, tmp;
//	for (i = 0; i < 4; i++)
//		for (j = 3; j >= 0; j--)
//			if (a[j][i] == 0)   //���н����±�λ��
//				for (z = j - 1; z >= 0; z--)
//					if (a[z][i] > 0)
//					{
//						tmp = a[j][i];
//						a[j][i] = a[z][i];
//						a[z][i] = tmp;
//						break;
//					}
//	for (i = 0; i < 4; i++)
//		for (j = 3; j >= 0; j--)
//			if (a[j][i] > 0)
//				for (z = j - 1; z >= 0; z--)
//					if (a[z][i] > 0)
//						if (a[z][i] == a[j][i])
//						{
//							a[j][i] *= 2;
//							a[z][i] = 0;
//						}
//						else
//							break;
//					else
//						break;
//			else
//				break;
//}
//void putt(char ch)
//{
//	switch (ch)
//	{
//	case 'a':
//		left();
//		break;
//	case 'd':
//		right();
//		break;
//	case 'w':
//		up();
//		break;
//	case 's':
//		down();
//		break;
//	}
//}
//int new()//��������
//{
//	srand((unsigned int) time(NULL));
//    int x = 0, y = 0, num = 0;
//	int fals = 1;
//    do {     
//        int i, j;
//        for (i = 0; i < 4; i++)
//            for (j = 0; j < 4; j++)
//                if (a[i][j] == 0)   //ֻҪ��λ�ñ�־λ������
//                    fals = 0;
//        if (fals) //�¸��޷������룬��Ϸʧ�� 
//        {
//            system("cls");
//			return 1;
//        }
//        x = rand() % 4;
//        y = rand() % 4;
//        num = rand() % 2;  
//		int k = rand() % 10;
//        if (num == 0)
//            num = b[k];
//        else if (num == 1)
//            num = b[k];
//    } while (a[x][y] > 0); //��0�Żᱻ����ֵ
//    x1 = x;
//    y1 = y;
//    num1 = num;
//    a[x1][y1] = num1;
//}
//int main44()
//{
//	printf("���������ַ���ʼ,��ESC���˳�\n");
//	create(2);	
//	interface_2048();
//	while (1)
//	{
//		char ch = _getch();
//		if (ch == 'a' || ch == 'd' || ch == 's' || ch == 'w')
//		{
//			system("cls");
//			putt(ch);
//			interface_2048();
//			int i=new();
//			if (1 == i)
//				break;
//		}
//		if (ch == 27)
//		{
//			system("cls");
//			break;
//		}
//	}
//	printf("game over!!!!!!!!\n");
//	system("pause");
//	return 0;
//}
