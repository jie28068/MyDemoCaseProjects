#if 0
#define _CRT_SECURE_NO_WARNINGS
#include <vector>
#include <iostream>
using namespace std;
/*
   * User:SJ
   * Time:
   * Function:���ֲ���һ����������һ�������������Ƿ��и�ֵ���оͱ�ǣ�������һ���������
   * Explain:ʹ�ö��ֲ��ҵĹؼ���������������
 */
class solution
{
public:
	/*��ʽһ��ѭ��*/
	vector<int> search_array(vector<int>& sort_array, vector<int> & random_array)
	{
		/**����һ����������*/
		vector<int> result;
		/*��ʼ��*/
		for (int i = 0; i < random_array.size(); i++)
		{
			result.push_back(0);
		}
		
		/*һ������ȥ����*/
		for (int i = 0; i < random_array.size(); i++)
		{
			int begin = 0;
			int end = sort_array.size() - 1;
			/*���ֲ���*/
			while (begin <= end)
			{
				int mid = (begin + end) / 2;
				if (random_array[i] == sort_array[mid])
				{
					/*�ҵ��ˣ����һ�²��˳�*/
					result[i] += 1;
					break;
				}
				else if (random_array[i] < sort_array[mid])
				{
					end = mid - 1;
				}
				else if (random_array[i] > sort_array[mid])
				{
					begin = mid + 1;
				}
			}
		}
		return result;
	}
};
/*��ʽ�����ݹ�*/
bool binary_sreach(vector<int>& sort_array, int begin, int end, int target)
{
	if (begin > end)
	{
		return false;
	}
	int mid = (begin + end) / 2;
	if (target == sort_array[mid])
	{
		return true;
	}
	else if (target < sort_array[mid])
	{
		return binary_sreach(sort_array, begin, mid-1, target);
	}
	else if (target > sort_array[mid])
	{
		return binary_sreach(sort_array, mid + 1, end, target);
	}
}
int main()
{
	solution s;
	vector<int> sort_array; 
	vector<int> random_array;
	int a[] = { -1,2,5,20,90,100,207,800 };
	int b[] = { 50,90,3,-1,207,80 };
	for (int i = 0; i < 8; i++)
	{
		sort_array.push_back(a[i]);
	}
	for (int i = 0; i < 6; i++)
	{
		random_array.push_back(b[i]);
	}
	vector<int> result = s.search_array(sort_array, random_array);
	for (int i = 0; i < result.size(); i++)
	{
		cout << result[i] << " ";
	}
	system("pause");
	return  EXIT_SUCCESS;
}
#endif
