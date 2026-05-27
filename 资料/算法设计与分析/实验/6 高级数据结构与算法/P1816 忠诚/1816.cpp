#include <iostream>
#include <cmath>
#include <algorithm>
using namespace std;

#define MAX 100005

int n, m;
int f[MAX][18];

int query(int l, int r)
{
    int k = log2(r - l + 1);
    return min(f[l][k], f[r - (1 << k) + 1][k]);
}

int main()
{
    cin >> n >> m;

    for (int i = 1; i <= n; i++)
    {
        cin >> f[i][0];
    }

    for (int j = 1; (1 << j) <= n; j++)
    {
        for (int i = 1; i + (1 << j) - 1 <= n; i++)
        {
            f[i][j] = min(f[i][j - 1], f[i + (1 << (j - 1))][j - 1]);
        }
    }

    for (int i = 0; i < m; i++)
    {
        int l, r;
        cin >> l >> r;
        cout << query(l, r) << " ";
    }

    return 0;
}