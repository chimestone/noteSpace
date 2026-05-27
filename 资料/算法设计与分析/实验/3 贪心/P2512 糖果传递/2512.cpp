
#include <iostream>
#include <algorithm>
using namespace std;
#define MAX 1000050

int main(void)
{
    int n, arr[MAX];
    long long sum = 0;
    cin >> n;
    for (int i = 0; i < n; i++)
    {
        cin >> arr[i];
        sum += arr[i];
    }
    
    long long target = sum / n;
    long long c[MAX];
    
    c[0] = 0;
    for (int i = 1; i < n; i++)
    {
        c[i] = c[i-1] + arr[i-1] - target;
    }
    
    sort(c, c + n);
    
    long long mid = c[n/2];
    long long ans = 0;

    for (int i = 0; i < n; i++)
    {
        ans += abs(c[i] - mid);
    }
    
    cout << ans << endl;
    return 0;
}