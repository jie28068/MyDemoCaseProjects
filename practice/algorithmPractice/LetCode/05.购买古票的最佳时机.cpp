#if 0
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
using namespace std;
/*
   * User:SJ
   * Time:
   * Function:
   * Explain:
 */

int maxProfit(int* prices, int pricesSize) {
    int ans = 0;
    for (int i = 1; i < pricesSize; ++i) {
        int a = prices[i] - prices[i - 1];
        if (a > 0)
        {
            ans += a;
        }
    }
    return ans;
}
int main()
{
    int a[] = { 8,9,2,5,4,7,1 };
    int p = maxProfit(a, 7);
    cout << p << endl;
	system("pause");
	return  EXIT_SUCCESS;
}
#endif
