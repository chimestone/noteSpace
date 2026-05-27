#include <stdio.h>
#include <string.h>

int min(int a, int b) {
    return a < b ? a : b;
}

int main() {
    int n, c;
    scanf("%d%d", &n, &c);
    
    int a[55], b[55], sum[55];
    int f[55][55][2];
    
    memset(f, 127, sizeof(f));
    
    for(int i = 1; i <= n; i++) {
        scanf("%d%d", &a[i], &b[i]);
        sum[i] = sum[i-1] + b[i];
    }
    f[c][c][0] = f[c][c][1] = 0;
    
    for(int len = 2; len <= n; len++) {
        for(int i = 1; i + len - 1 <= n; i++) {
            int j = i + len - 1;
            
            int power = sum[i] + sum[n] - sum[j];
            f[i][j][0] = min(f[i+1][j][0] + (a[i+1] - a[i]) * power,
                           f[i+1][j][1] + (a[j] - a[i]) * power);
            f[i][j][1] = min(f[i][j-1][0] + (a[j] - a[i]) * (sum[i-1] + sum[n] - sum[j-1]),
                           f[i][j-1][1] + (a[j] - a[j-1]) * (sum[i-1] + sum[n] - sum[j-1]));
        }
    }
    
    printf("%d\n", min(f[1][n][0], f[1][n][1]));
    
    return 0;
}