/*
f记录花束数 v记录花瓶数
很像背包啊（bushi
从第一朵花开始
我们使用f[i][j]表示前i朵花放在j瓶子里面的最大美学值
然后第i+1朵花 f[i+1][j] = max(从前到后的遍历得到f[i][k] + a[k][j])
*/
#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    int f, v;
    scanf("%d %d", &f, &v);
    int **arr = (int **)malloc(sizeof(int **) * f);
    for (int i = 0; i < f; i++)
    {
        arr[i] = (int *)malloc(sizeof(int) * v);
    }

    for(int i=0; i<f; i++) {
        for(int j=0; j<v; j++) {
            scanf("%d",&arr[i][j]);
        }
    }
    int mins = -0x3f3f3f3f;
    int fun[101][101];
    int path[101][101];
    
    for(int i=0; i<=f; i++) {
        for(int j=0; j<=v; j++) {
            fun[i][j] = mins;
            path[i][j] = -1;
        }
    }
    
    for(int i=0; i<v; i++) {
        fun[0][i] = arr[0][i];
    }
    
    for(int i=1; i<f; i++) {
        for(int j=i; j<v; j++) {
            for(int k=i-1; k<j; k++) {
                if(fun[i-1][k] != mins) {
                    if(fun[i][j] < fun[i-1][k] + arr[i][j]) {
                        fun[i][j] = fun[i-1][k] + arr[i][j];
                        path[i][j] = k;
                    }
                }
            }
        }
    }
    
    int maxVal = mins, maxPos = -1;
    for(int j=f-1; j<v; j++) {
        if(fun[f-1][j] > maxVal) {
            maxVal = fun[f-1][j];
            maxPos = j;
        }
    }
    
    printf("%d\n", maxVal);
    
    int result[101];
    int pos = maxPos;
    for(int i=f-1; i>=0; i--) {
        result[i] = pos + 1;
        pos = path[i][pos];
    }
    
    for(int i=0; i<f; i++) {
        printf("%d ", result[i]);
    }
    printf("\n");
    
    for(int i=0; i<f; i++) {
        free(arr[i]);
    }
    free(arr);
    
    return 0;
}