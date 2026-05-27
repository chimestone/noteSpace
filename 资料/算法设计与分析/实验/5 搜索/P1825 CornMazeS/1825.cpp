#include<iostream>
#include<queue>
#include<string>
using namespace std;

struct point {
    int x, y, t;
};

char a[350][350];
bool vis[350][350];
int n, m;
int dx[4] = {1, 0, -1, 0};
int dy[4] = {0, 1, 0, -1};
int sx, sy;

void goto_another(int &nx, int &ny) {
    for(int i = 1; i <= n; i++) {
        for(int j = 1; j <= m; j++) {
            if(a[i][j] == a[nx][ny] && (i != nx || j != ny)) {
                nx = i;
                ny = j;
                return;
            }
        }
    }
}

int main() {
    cin >> n >> m;
    string s;
    
    for(int i = 1; i <= n; i++) {
        cin >> s;
        for(int j = 1; j <= m; j++) {
            a[i][j] = s[j-1];
            if(a[i][j] == '@') {
                sx = i;
                sy = j;
            }
        }
    }
    
    queue<point> que;
    que.push({sx, sy, 0});
    
    while(!que.empty()) {
        point f = que.front();
        que.pop();
        
        if(a[f.x][f.y] == '=') {
            cout << f.t;
            return 0;
        }
        
        if(a[f.x][f.y] >= 'A' && a[f.x][f.y] <= 'Z') {
            goto_another(f.x, f.y);
        }
        
        for(int i = 0; i < 4; i++) {
            int nx = f.x + dx[i];
            int ny = f.y + dy[i];
            
            if(nx >= 1 && nx <= n && ny >= 1 && ny <= m && a[nx][ny] != '#' && !vis[nx][ny]) {
                vis[nx][ny] = true;
                que.push({nx, ny, f.t + 1});
            }
        }
    }
    
    return 0;
}