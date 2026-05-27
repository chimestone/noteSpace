#include <stdio.h>
#include <stdlib.h>

typedef struct
{
    int flow, step;
} node;

int *k;

void bfs(int n, int a, int *arr, int step)
{
    node *queue = (node *)malloc(sizeof(node) * n * 10);
    int front = 0, rear = 0;
    queue[rear++] = (node){a, step};
    arr[a] = step;

    while (front < rear)
    {
        node cur = queue[front++];
        if (k[cur.flow] == 0)
            continue;
        for (int i = -1; i < 2; i += 2)
        {
            int cur_a = cur.flow + i * k[cur.flow];
            if (cur_a >= 0 && cur_a < n)
            {
                if (arr[cur_a] == -1)
                {
                    queue[rear++] = (node){cur_a, cur.step + 1};
                    arr[cur_a] = cur.step + 1;
                }
            }
        }
    }
    free(queue);
}

int main(void)
{
    int n, a, b;
    scanf("%d %d %d", &n, &a, &b);
    k = (int *)malloc(sizeof(int) * n);
    for (int i = 0; i < n; i++)
    {
        scanf("%d", &k[i]);
    }

    int *arr = (int *)malloc(sizeof(int) * n);
    for (int i = 0; i < n; i++)
        arr[i] = -1;

    bfs(n, a - 1, arr, 0);

    printf("%d", arr[b - 1]);

    free(k);
    free(arr);
    k = NULL;
    arr = NULL;
    return 0;
}
