/*
bfs搜索 地图变动

*/

#include <iostream>
#include <cstring>
#include <queue>

#define MAX 9999999

using namespace std;

int m;
int map[301][301];
int visited[301][301];
int ans[301][301];

int dx[] = {0, 0, 0, 1, -1};
int dy[] = {0, 1, -1, 0, 0};

int sx, sy, st;

int ch(int a)
{
    if (a == -1)
        return MAX;
    else
        return a;
}

int main(void)
{
    cin >> m;
    memset(map, -1, sizeof(map));

    for (int i = 0; i < m; i++)
    {
        cin >> sx >> sy >> st;
        for (int j = 0; j < 5; j++)
        {
            if (sx + dx[j] >= 0 && sy + dy[j] >= 0 && (map[sx + dx[j]][sy + dy[j]] == -1 || map[sx + dx[j]][sy + dy[j]] > st))
            {
                map[sx + dx[j]][sy + dy[j]] = st;
            }
        }
    } 

    queue<int> q[2];
    visited[0][0] = 1;
    q[0].push(0);
    q[1].push(0);
    while (!q[0].empty())
    {
        int x = q[0].front();
        int y = q[1].front();

        q[0].pop();
        q[1].pop();

        int s = ans[x][y] + 1;
        if (map[x][y] == -1)
        {
            cout << s - 1 << endl;
            return 0;
        }
        for (int i = 1; i < 5; i++)
        {
            int cur_x = x + dx[i];
            int cur_y = y + dy[i];

            if(cur_x >= 0 && cur_y >= 0 && s<ch(map[cur_x][cur_y]) && visited[cur_x][cur_y] == 0) {
                q[0].push(cur_x);
                q[1].push(cur_y);
                visited[cur_x][cur_y] = 1;
                ans[cur_x][cur_y] = s;
            }
        }
    }
    cout << -1 << endl;
    return 0;
}