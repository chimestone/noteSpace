# SPFA算法详解

## 1. 什么是SPFA？

**SPFA (Shortest Path Faster Algorithm)** 是求解单源最短路径的算法。

### 核心思想
- **SPFA = Bellman-Ford算法 + 队列优化**
- 只有当一个点的距离被更新了，它的邻居才可能需要更新
- 用队列维护"可能需要更新的点"，避免无效计算

## 2. 基础概念：松弛操作

### 什么是松弛？
想象你要找从家到学校的最短路径：

- 你知道从家到A点的最短距离是10
- A点到B点有一条路，长度是5
- 那么从家到B点，最多不超过10+5=15

如果你之前以为从家到B点要走20，现在发现只要15，你就**更新**这个距离。

这个"更新更短距离"的过程，就叫**松弛**。

### 松弛操作的代码表示
```cpp
if (dis[v] > dis[u] + edge_weight) {
    dis[v] = dis[u] + edge_weight;  // 松弛成功
}
```

## 3. SPFA算法流程

### 初始化
```cpp
memset(dis, 0x3f, sizeof(dis));  // 距离初始化为无穷大
dis[s] = 0;                      // 源点距离为0
vis[s] = true;                   // 标记源点在队列中
q.push(s);                       // 源点入队
```

### 主循环
```cpp
while (!q.empty()) {
    int u = q.front(); q.pop();  // 取出队首
    vis[u] = false;              // 标记出队
    
    // 遍历u的所有邻接点
    for (int i = fir[u]; i; i = e[i].nxt) {
        int v = e[i].to;
        if (dis[v] > dis[u] + e[i].val) {  // 松弛操作
            dis[v] = dis[u] + e[i].val;
            if (!vis[v]) {       // 如果v不在队列中
                vis[v] = true;   // 标记入队
                q.push(v);       // v入队
            }
        }
    }
}
```

## 4. 负环检测

### 什么是负环？
想象地图上有这样一个环路：
- A → B，距离是10
- B → C，距离是5  
- C → A，距离是-20

你从A出发，走一圈回到A：10 + 5 + (-20) = -5

走了一圈，不但没花距离，还"赚了"5的距离！

### 负环的问题
如果存在负环，你就可以在这个环里无限转圈，让距离变成负无穷。
这时候"最短路径"就没有意义了。

### 检测方法
- 记录每个点进入队列的次数
- 如果某个点被松弛了n次或更多，说明存在负环
- 因为如果没有负环，每个点最多被松弛n-1次

```cpp
cnt[v]++;                    // 记录入队次数
if (cnt[v] >= n) {          // 入队次数达到n
    return false;           // 存在负环
}
```

## 5. 差分约束系统

### 问题转化
小K的农场问题不是找路径，而是约束求解问题！

给定约束条件，问能否找到一组数字x[1], x[2], ..., x[n]同时满足所有约束？

### 三种约束的转换

| 约束类型 | 数学表示 | 转化为边 | 说明 |
|---------|---------|---------|------|
| 至少约束 | `x[a] - x[b] ≥ c` | `b → a`，权值 `c` | 农场a比农场b至少多c个 |
| 至多约束 | `x[a] - x[b] ≤ c` | `a → b`，权值 `-c` | 农场a比农场b至多多c个 |
| 相等约束 | `x[a] = x[b]` | `a ↔ b`，权值都是 `0` | 农场a与农场b一样多 |

### 转换原理
SPFA求出的最短距离dis[i]就相当于x[i]的值。

- 如果从b到a有权值为c的边，SPFA保证：`dis[a] ≥ dis[b] + c`
- 即：`x[a] - x[b] ≥ c` ✓

### 为什么负环=约束矛盾？
如果有负环，说明存在一系列约束，绕一圈后产生矛盾。

例如：A≥B+5, B≥C+3, C≥A+1
绕一圈：A≥B+5≥C+8≥A+9，即A≥A+9，矛盾！

## 6. 超级源点

### 为什么需要？
- SPFA需要一个起点，但题目没有明确起点
- 需要确保所有点都能被访问到，才能检测所有可能的负环

### 实现方法
```cpp
int s = n + 1;  // 超级源点
for(int i = 1; i <= n; i++) {
    add(s, i, 0);  // 从源点到每个农场连权值0的边
}
```

权值为0是因为不想让虚拟边影响原有约束关系。

## 7. 完整解题流程

1. **读取约束**：根据指令类型建立对应的边
2. **建立超级源点**：连接所有农场
3. **运行SPFA**：检测负环
4. **输出结果**：
   - 有负环 → "No"（约束矛盾，无解）
   - 无负环 → "Yes"（约束一致，有解）

## 8. 代码模板

```cpp
#include<iostream>
#include<cstdio>
#include<queue>
#include<string.h>
using namespace std;

int n,m;
int tot,fir[10005];
struct edge {
    int nxt,to,val;
} e[15005];

void add(int u,int v,int w) {
    e[++tot]={fir[u],v,w};
    fir[u]=tot;
}

int dis[15005],cnt[15005];
bool vis[15005];
queue<int>q;

bool SPFA(int s) {
    memset(dis,0x3f,sizeof dis);
    vis[s]=1;
    dis[s]=0;
    q.push(s);
    
    while(!q.empty()) {
        int u=q.front();q.pop();
        vis[u]=0;
        
        for(int i=fir[u];i;i=e[i].nxt) {
            int v=e[i].to;
            if(dis[v]>dis[u]+e[i].val) {
                dis[v]=dis[u]+e[i].val;
                cnt[v]++;
                if(cnt[v]==n) return 0;  // 负环
                if(vis[v]==0) {
                    vis[v]=1;
                    q.push(v);
                }
            }
        }
    }
    return 1;
}

int main() {
    scanf("%d%d",&n,&m);
    
    // 读取约束并建边
    for(int i=0;i<m;i++) {
        int op,a,b,c;
        scanf("%d",&op);
        if(op==1) {
            scanf("%d%d%d",&a,&b,&c);
            add(b,a,c);  // x[a]-x[b]>=c
        }
        else if(op==2) {
            scanf("%d%d%d",&a,&b,&c);
            add(a,b,-c); // x[a]-x[b]<=c
        }
        else {
            scanf("%d%d",&a,&b);
            add(a,b,0);  // x[a]=x[b]
            add(b,a,0);
        }
    }
    
    // 超级源点
    int s=n+1;
    for(int i=1;i<=n;i++) {
        add(s,i,0);
    }
    
    if(SPFA(s)) cout<<"Yes";
    else cout<<"No";
    
    return 0;
}
```

## 9. 总结

SPFA的核心是**松弛操作**和**队列优化**：
- 松弛：尝试通过当前点更新邻接点距离
- 队列：只处理可能需要更新的点
- 负环检测：入队次数≥n次
- 差分约束：将约束转化为图上的边

记住：**有负环 = 约束矛盾 = 无解**