#include <stdio.h>
#include <stdlib.h>

long long tree[500005];
int n;

int lowbit(int x)
{
    return x & (-x);
}

void update(int x, long long k)
{
    for (int i = x; i <= n; i += lowbit(i))
    {
        tree[i] += k;
    }
}

long long query(int x)
{
    long long res = 0;
    for (int i = x; i > 0; i -= lowbit(i))
    {
        res += tree[i];
    }
    return res;
}

long long rangeQuery(int l, int r)
{
    return query(r) - query(l - 1);
}

int main(void)
{
    int m;
    scanf("%d %d", &n, &m);

    for (int i = 0; i <= n; i++)
    {
        tree[i] = 0;
    }

    for (int i = 1; i <= n; i++)
    {
        long long x;
        scanf("%lld", &x);
        update(i, x);
    }
    for (int i = 0; i < m; i++)
    {
        int op;
        scanf("%d", &op);

        if (op == 1)
        {
            int x;
            long long k;
            scanf("%d %lld", &x, &k);
            update(x, k);
        }
        else
        {
            int l, r;
            scanf("%d %d", &l, &r);
            printf("%lld\n", rangeQuery(l, r));
        }
    }

    return 0;
}
