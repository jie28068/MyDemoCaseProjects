#if 0
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Windows.h>
/*
   * User:SJ
   * Time:
   * Function:
   * Explain:
 */
int func4(int* nums, int numsSize)
{
	int left = 0;
	int right = 1;
	if (numsSize <= 0)
	{
		return 0;
	}
	for (int i = 1; i < numsSize; i++)
	{
		if (nums[left] != nums[right])    //�жϿ���ָ���Ƿ���ͬ��
		{	
			nums[left + 1] = nums[right];//��ͬ�Ͱѿ�ĸ�ֵ�����ĺ�һ��
			left++;
			right++;//��ָ�붼�ƶ�һ��
		}
		else//����ֻ�ƶ���ָ��
		{
			right++;
		}
	}
	return left;
}
int removeDuplicates(int* nums, int numsSize) {
	if (numsSize == 0) {
		return 0;
	}
	int fast = 1, slow = 1;
	while (fast < numsSize) {
		if (nums[fast] != nums[fast - 1]) {
			nums[slow] = nums[fast];
			++slow;
		}
		++fast;
	}
	return slow;
}
int main04()
{
	int a[] = { 1,1,2};
	int n= func4(a, 3);
	for (int i = 0; i < n; ++i)
	{
		printf("%d\t", a[i]);
	}
	system("pause");
	return 0;
}
#endif