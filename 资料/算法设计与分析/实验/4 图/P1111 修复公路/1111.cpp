#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

// 加快输入输出（对于大数据量很重要）
void fastIO()
{
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    cout.tie(nullptr);
}

vector<int> parent;
vector<int> ranks;
int cnts;

// 非递归find，避免栈溢出
int find(int x)
{
    int root = x;
    while (root != parent[root])
    {
        root = parent[root];
    }
    // 路径压缩
    while (x != root)
    {
        int next = parent[x];
        parent[x] = root;
        x = next;
    }
    return root;
}

void merge(int x, int y)
{
    int fx = find(x);
    int fy = find(y);

    if (fx == fy)
        return;

    cnts--;

    if (ranks[fx] < ranks[fy])
    {
        parent[fx] = fy;
    }
    else if (ranks[fx] > ranks[fy])
    {
        parent[fy] = fx;
    }
    else
    {
        parent[fy] = fx;
        ranks[fx]++;
    }
}

int main()
{
    fastIO();

    int n, m;
    cin >> n >> m;

    // 边界情况处理
    if (n <= 1)
    {
        cout << 0 << endl;
        return 0;
    }

    if (m == 0)
    {
        cout << -1 << endl;
        return 0;
    }

    // 初始化并查集
    parent.resize(n);
    ranks.resize(n, 0);
    cnts = n;

    for (int i = 0; i < n; i++)
    {
        parent[i] = i;
    }

    // 存储边
    vector<pair<int, pair<int, int>>> edges;
    edges.reserve(m); // 预分配内存

    // 读取边
    for (int i = 0; i < m; i++)
    {
        int curx, cury, curt;
        cin >> curx >> cury >> curt;

        // 假设输入节点编号从1开始，转换为0-based
        // 如果输入是0-based，注释掉下面两行
        curx--;
        cury--;

        // 验证节点编号有效性
        if (curx < 0 || curx >= n || cury < 0 || cury >= n)
        {
            // 非法输入，跳过或处理
            continue;
        }

        edges.push_back({curt, {curx, cury}});
    }

    // 如果没有有效边
    if (edges.empty())
    {
        cout << -1 << endl;
        return 0;
    }

    // 按时间排序
    sort(edges.begin(), edges.end());

    int earliestTime = -1;

    // 按时间顺序处理边
    for (auto &edge : edges)
    {
        int curt = edge.first;
        int curx = edge.second.first;
        int cury = edge.second.second;

        merge(curx, cury);

        if (cnts == 1)
        {
            earliestTime = curt;
            break;
        }
    }

    cout << earliestTime << endl;
    return 0;
}