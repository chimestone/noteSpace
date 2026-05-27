#include <stdio.h>
#include <stdlib.h>

int dx[] = {-2, -2, -1, -1, 1, 1, 2, 2};
int dy[] = {-1, 1, -2, 2, -2, 2, -1, 1};

int isIn(int x, int y, int n, int m)
{
    if (x >= 0 && y >= 0 && x < n && y < m)
        return 1;
    return 0;
}

typedef struct node
{
    int x, y, step;
} node;

void bfs(int x, int y, int n, int m, int **arr, int step)
{
    node *queue = (node *)malloc(n * m * sizeof(node));
    arr[x][y] = step;
    int front = 0;
    int rear = 0;
    queue[rear++] = (node){x, y, step};

    while (front < rear)
    {
        node cur = queue[front++];
        for (int i = 0; i < 8; i++)
        {
            int cur_x = cur.x + dx[i];
            int cur_y = cur.y + dy[i];
            if (isIn(cur_x, cur_y, n, m) && arr[cur_x][cur_y] == -1)
            {
                queue[rear++] = (node){cur_x, cur_y, cur.step + 1};
                arr[cur_x][cur_y] = cur.step + 1;
            }
        }
    }
    free(queue);
}

int main(void)
{
    int n, m, x, y;
    scanf("%d %d %d %d", &n, &m, &x, &y);

    int **arr = (int **)malloc(sizeof(int *) * n);
    for (int i = 0; i < n; i++)
    {
        arr[i] = (int *)malloc(m * sizeof(int));
        for (int j = 0; j < m; j++)
        {
            arr[i][j] = -1;
        }
    }

    bfs(x - 1, y - 1, n, m, arr, 0);
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < m; j++)
        {
            printf("%d ", arr[i][j]);
        }
        printf("\n");
    }

    for (int i = 0; i < n; i++)
    {
        free(arr[i]);
        arr[i] = NULL;
    }
    free(arr);
    arr = NULL;
    return 0;
}