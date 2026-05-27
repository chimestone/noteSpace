#include <iostream>
#include <cstdlib> // 用于abs函数
using namespace std;

const int MAXN = 30005;

int parent[MAXN];
int dist[MAXN];
int Size[MAXN];

// 初始化
void init(int n)
{
    for (int i = 1; i <= n; i++)
    {
        parent[i] = i;
        dist[i] = 0;
        Size[i] = 1;
    }
}

int find(int x)
{
    if (parent[x] != x)
    {
        int root = find(parent[x]);
        dist[x] += dist[parent[x]];
        parent[x] = root;
    }
    return parent[x];
}

void merge(int i, int j)
{
    int root_i = find(i);
    int root_j = find(j);

    parent[root_i] = root_j;

    dist[root_i] = Size[root_j];

    Size[root_j] += Size[root_i];
}

int query(int i, int j)
{
    if (find(i) != find(j))
    {
        return -1;
    }
    return abs(dist[i] - dist[j]) - 1;
}

int main()
{
    int T;
    cin >> T;

    init(30000);

    while (T--)
    {
        char op;
        int i, j;
        cin >> op >> i >> j;

        if (op == 'M')
        {
            merge(i, j);
        }
        else
        {
            cout << query(i, j) << endl;
        }
    }

    return 0;
}