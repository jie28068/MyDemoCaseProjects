#include <stdio.h>
/*回调函数现实冒泡排序*/
#include "pao38.h"

void swap(int* a, int* b)
{
    int tmp = *a;
    *a = *b;
    *b = tmp;
}
void huidiao(void* arr, int len, int size, void(*print)(void*))
{
    char* temp = (char*)arr; //强转为char类型方便，地址偏移
    for (int i = 0; i < len; i++)
    {
        print(temp + i * size);//传递一个地址偏移量过去
    }
}
void myfunc38(void* data)
{
    int* temp = (int*)data;//转换为原有的数据类型
    printf("%d\t", *temp);
}
int main38()
{
    int arr[] = { 130,11, 202, 49, 58, 7, 96, 48, 100, 9 };
    int len = sizeof(arr) / sizeof(arr[0]);
    printf("排序前：\n");
    for (int i = 0; i < len; i++)
    {
        printf("%d ", arr[i]);
    }
    printf("\n");
    pao38(swap, arr, len);      //调用回调函数
    printf("排序后：\n");
    //for (int i = 0; i < len; i++)
    //{
    //    printf("%d ", arr[i]);
    //}
    //printf("\n");
    //利用回调函数输出，传递参数为：数组名，长度，字节长度，函数名
    huidiao(arr, sizeof(arr) / sizeof(arr[0]), sizeof(int), myfunc38);

    system("pause");
    return 0;
}
