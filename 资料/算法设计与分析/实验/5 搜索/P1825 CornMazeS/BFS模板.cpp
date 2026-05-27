#include <iostream>
#include <queue>
#include <cstring>
using namespace std;

// BFS基础模板
int n, m;
char grid[1005][1005];
bool vis[1005][1005];
int dx[] = {-1, 1, 0, 0};  // 上下左右
int dy[] = {0, 0, -1, 1};

struct Node {
    int x, y, step;
};

bool isValid(int x, int y) {
    return x >= 0 && x < n && y >= 0 && y < m && grid[x][y] != '#' && !vis[x][y];
}

int bfs(int sx, int sy, int ex, int ey) {
    queue<Node> q;
    memset(vis, false, sizeof(vis));
    
    q.push({sx, sy, 0});
    vis[sx][sy] = true;
    
    while (!q.empty()) {
        Node cur = q.front();
        q.pop();
        
        // 到达终点
        if (cur.x == ex && cur.y == ey) {
            return cur.step;
        }
        
        // 四个方向扩展
        for (int i = 0; i < 4; i++) {
            int nx = cur.x + dx[i];
            int ny = cur.y + dy[i];
            
            if (isValid(nx, ny)) {
                vis[nx][ny] = true;
                q.push({nx, ny, cur.step + 1});
            }
        }
    }
    
    return -1;  // 无法到达
}

int main() {
    cin >> n >> m;
    
    int sx, sy, ex, ey;  // 起点和终点
    
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            cin >> grid[i][j];
            if (grid[i][j] == 'S') {  // 起点
                sx = i; sy = j;
            }
            if (grid[i][j] == 'E') {  // 终点
                ex = i; ey = j;
            }
        }
    }
    
    int result = bfs(sx, sy, ex, ey);
    
    if (result == -1) {
        cout << "无法到达" << endl;
    } else {
        cout << "最短步数: " << result << endl;
    }
    
    return 0;
}