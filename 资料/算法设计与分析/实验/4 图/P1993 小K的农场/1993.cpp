/*
n个 农场
m个 信息

1 农场a比农场b至少多种植了c个单位的作物 -> b到a连边权值c
2 农场a比农场b至多多种植了c个单位的作物 -> a到b连边权值-c
3 农场a与农场b种植的作物一样多 -> a和b互相连边权值0

判断是否存在一种吻合的情况
负环判断
*/
#include <iostream>
#include <cstdio>
#include <queue>
#include <cstring>
using namespace std;

int n, m;
int tot, fir[100005];
struct edge {
    int nxt, to, val;
} e[300005];

void add(int u, int v, int w) {
    e[++tot] = {fir[u], v, w};
    fir[u] = tot;
}

int dis[100005], cnt[100005];
bool vis[100005];
queue<int> q;

bool SPFA(int s) {
    memset(dis, 0x3f, sizeof(dis));
    vis[s] = 1;
    dis[s] = 0;
    q.push(s);
    
    while (!q.empty()) {
        int u = q.front();
        q.pop();
        vis[u] = 0;
        
        for (int i = fir[u]; i; i = e[i].nxt) {
            int v = e[i].to;
            if (dis[v] > dis[u] + e[i].val) {
                dis[v] = dis[u] + e[i].val;
                cnt[v]++;
                if (cnt[v] > n) return 0;  // 负环
                if (vis[v] == 0) {
                    vis[v] = 1;
                    q.push(v);
                }
            }
        }
    }
    return 1;
}

int main(void) {
    scanf("%d%d", &n, &m);
    
    for (int i = 0; i < m; i++) {
        int op;
        scanf("%d", &op);
        if (op == 1) {
            int a, b, c;
            scanf("%d%d%d", &a, &b, &c);
            add(a, b, -c);  // x[a] - x[b] >= c -> x[b] - x[a] <= -c
        }
        else if (op == 2) {
            int a, b, c;
            scanf("%d%d%d", &a, &b, &c);
            add(b, a, c); // x[b] - x[a] <= c
        }
        else {
            int a, b;
            scanf("%d%d", &a, &b);
            add(a, b, 0);  // x[a] = x[b]
            add(b, a, 0);
        }
    }
    
    // 超级源点
    int s = n + 1;
    for (int i = 1; i <= n; i++) {
        add(s, i, 0);
    }
    
    if (SPFA(s)) printf("Yes");
    else printf("No");
    
    return 0;
}