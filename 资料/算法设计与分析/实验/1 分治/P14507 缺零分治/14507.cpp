#include <iostream>
#include <algorithm>
using namespace std;

typedef long long ll;

const int N = 100010;
ll bucket[N];
ll sizes[N], cnt[N], prefix[N], prefixCnt[N], total;

struct Node
{
    ll left, right, minVal;
    ll lazy;
} tree[N << 2];

void update(int p)
{
    tree[p].minVal = min(tree[p * 2].minVal, tree[p * 2 + 1].minVal);
}

void apply(int p, ll v)
{
    tree[p].lazy += v;
    tree[p].minVal += v;
}

void pushDown(int p)
{
    if (tree[p].lazy)
    {
        apply(p * 2, tree[p].lazy);
        apply(p * 2 + 1, tree[p].lazy);
        tree[p].lazy = 0;
    }
}

void build(int p, int l, int r)
{
    tree[p].left = l;
    tree[p].right = r;
    tree[p].lazy = 0;
    if (l == r)
    {
        tree[p].minVal = bucket[l];
        return;
    }
    int mid = (l + r) / 2;
    build(p * 2, l, mid);
    build(p * 2 + 1, mid + 1, r);
    update(p);
}

void change(int p, int l, int r, ll v)
{
    if (tree[p].left == l && tree[p].right == r)
    {
        apply(p, v);
        return;
    }
    pushDown(p);
    if (r <= tree[p * 2].right)
        change(p * 2, l, r, v);
    else if (l >= tree[p * 2 + 1].left)
        change(p * 2 + 1, l, r, v);
    else
    {
        change(p * 2, l, tree[p * 2].right, v);
        change(p * 2 + 1, tree[p * 2 + 1].left, r, v);
    }
    update(p);
}

ll ask(int p, int l, int r)
{
    if (tree[p].left == l && tree[p].right == r)
        return tree[p].minVal;
    pushDown(p);
    if (r <= tree[p * 2].right)
        return ask(p * 2, l, r);
    else if (l >= tree[p * 2 + 1].left)
        return ask(p * 2 + 1, l, r);
    else
        return min(ask(p * 2, l, tree[p * 2].right), ask(p * 2 + 1, tree[p * 2 + 1].left, r));
}

int findZero(int p)
{
    if (tree[p].left == tree[p].right)
    {
        if (tree[p].minVal)
            return tree[p].right;
        else
            return tree[p].right - 1;
    }
    pushDown(p);
    if (tree[p * 2].minVal == 0)
        return findZero(p * 2);
    else
        return findZero(p * 2 + 1);
}

void solve()
{
    total = 0;
    int n, q;
    cin >> n >> q;
    bool valid = true;
    ll last = -1, color, num;
    for (int i = 1; i <= n; i++)
    {
        cin >> color >> num;
        if (!valid)
            continue;
        if (color == last + 1)
        {
            bucket[color + 1] = num;
            last = color;
        }
        else
        {
            valid = false;
        }
    }
    if (last == -1)
    {
        for (int i = 1; i <= q; i++)
        {
            ll m;
            cin >> m;
            if (m == 0)
                cout << "1\n";
            else
                cout << "-1\n";
        }
        return;
    }
    build(1, 1, last + 1);
    while (true)
    {
        int pos = findZero(1);
        if (!pos)
            break;
        ll mn = ask(1, 1, pos);
        sizes[++total] = pos;
        cnt[total] = mn;
        prefixCnt[total] = prefixCnt[total - 1] + mn;
        prefix[total] = prefix[total - 1] + pos * mn;
        change(1, 1, pos, -mn);
    }
    while (q--)
    {
        ll m;
        cin >> m;
        if (m > prefix[total] || m == 0)
        {
            cout << "-1\n";
            continue;
        }
        int pos = upper_bound(prefix + 1, prefix + total + 1, m) - prefix;
        if (pos == total + 1)
        {
            cout << prefixCnt[total] << "\n";
            continue;
        }
        if (m < sizes[1])
        {
            cout << "2\n";
            continue;
        }
        ll ans = prefixCnt[pos - 1];
        m -= prefix[pos - 1];
        if (m >= sizes[pos])
        {
            ans += m / sizes[pos];
            m -= m / sizes[pos] * sizes[pos];
        }
        if (m > 0)
            ans++;
        cout << ans << "\n";
    }
}

int main()
{
    ios::sync_with_stdio(false);
    cin.tie(0);
    cout.tie(0);
    int T;
    cin >> T;
    while (T--)
        solve();
    return 0;
}