/*
通过运算可以证明
p1 在 p2之前 等价于
a1 b1 < a2 b2
那么我们可以通过这一点进行排序
现在我们存进来left right n
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct BigNum {
    int num[5005];
    int len;
};

struct BigNum multiply(struct BigNum a, int b) {
    struct BigNum res = {0};
    for(int i = 0; i < a.len; i++) {
        res.num[i] += a.num[i] * b;
    }
    res.len = a.len;
    for(int i = 0; i < res.len; i++) {
        res.num[i+1] += res.num[i] / 10000;
        res.num[i] %= 10000;
    }
    while(res.num[res.len]) {
        res.num[res.len+1] += res.num[res.len] / 10000;
        res.num[res.len] %= 10000;
        res.len++;
    }
    return res;
}

struct BigNum divide(struct BigNum a, int b) {
    struct BigNum res = {0};
    long long cur = 0;
    for(int i = a.len-1; i >= 0; i--) {
        cur = cur * 10000 + a.num[i];
        res.num[i] = cur / b;
        cur %= b;
    }
    res.len = a.len;
    while(res.len > 1 && res.num[res.len-1] == 0) res.len--;
    return res;
}

int compare(struct BigNum a, struct BigNum b) {
    if(a.len > b.len) return 1;
    if(a.len < b.len) return -1;
    for(int i = a.len-1; i >= 0; i--) {
        if(a.num[i] > b.num[i]) return 1;
        if(a.num[i] < b.num[i]) return -1;
    }
    return 0;
}

void switchArr(int a, int b, int *arr){
    int temp = arr[a];
    arr[a] = arr[b];
    arr[b] = temp;
}

int main(void) {
    int n;
    scanf("%d",&n);
    
    int *left, *right;
    left = (int *)malloc(sizeof(int)*(n+1));
    right = (int *)malloc(sizeof(int)*(n+1));
    for(int i=0; i<=n; i++) {
        scanf("%d %d", &left[i], &right[i]);
    }

    for(int i = 1; i < n; i++) {
        for(int j = i + 1; j <= n; j++) {
            if((long long)left[i] * right[i] > (long long)left[j] * right[j]) {
                switchArr(i, j, left);
                switchArr(i, j, right);
            }
        }
    }
    
    struct BigNum cur = {0};
    cur.num[0] = left[0];
    cur.len = 1;
    
    struct BigNum ans = {0};
    ans.len = 1;
    
    for(int i = 1; i <= n; i++) {
        struct BigNum temp = divide(cur, right[i]);
        if(compare(temp, ans) > 0) {
            ans = temp;
        }
        cur = multiply(cur, left[i]);
    }
    
    printf("%d", ans.num[ans.len-1]);
    for(int i = ans.len-2; i >= 0; i--) {
        printf("%04d", ans.num[i]);
    }
    
    free(left);
    free(right);
    return 0;
}