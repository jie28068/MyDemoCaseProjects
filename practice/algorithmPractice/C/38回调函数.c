#include <stdio.h>
/*�ص�������ʵð������*/
#include "pao38.h"

void swap(int* a, int* b)
{
    int tmp = *a;
    *a = *b;
    *b = tmp;
}
void huidiao(void* arr, int len, int size, void(*print)(void*))
{
    char* temp = (char*)arr; //ǿתΪchar���ͷ��㣬��ַƫ��
    for (int i = 0; i < len; i++)
    {
        print(temp + i * size);//����һ����ַƫ������ȥ
    }
}
void myfunc38(void* data)
{
    int* temp = (int*)data;//ת��Ϊԭ�е���������
    printf("%d\t", *temp);
}
int main38()
{
    int arr[] = { 130,11, 202, 49, 58, 7, 96, 48, 100, 9 };
    int len = sizeof(arr) / sizeof(arr[0]);
    printf("����ǰ��\n");
    for (int i = 0; i < len; i++)
    {
        printf("%d ", arr[i]);
    }
    printf("\n");
    pao38(swap, arr, len);      //���ûص�����
    printf("�����\n");
    //for (int i = 0; i < len; i++)
    //{
    //    printf("%d ", arr[i]);
    //}
    //printf("\n");
    //���ûص�������������ݲ���Ϊ�������������ȣ��ֽڳ��ȣ�������
    huidiao(arr, sizeof(arr) / sizeof(arr[0]), sizeof(int), myfunc38);

    system("pause");
    return 0;
}
