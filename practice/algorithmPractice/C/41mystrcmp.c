#include <stdio.h>
#include<string.h>
void Strcmp(char* str1, char* str2)
{
    while ((*str1 != '\0') && (*str1 == *str2))//从左到右逐个比较字符是否相同
    {
        str1++;
        str2++;
    }
    int t;
    t = *str1 - *str2;
    if (t == 0)
        printf("0\n");
    if (t > 0)
        printf("1\n");
    if (t < 0)
        printf("-1\n");
}
int main41()
{
    char str1[10], str2[10];
    gets(str1);
    gets(str2);
    Strcmp(str1, str2);
    system("pause");
    return 0;

}