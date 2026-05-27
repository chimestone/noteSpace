/*
给定一个长度为5的木板
给定预期的涂色目标
给出最少的涂色次数

考虑这样的一个区间
当区间长度为1时显然方法数为1
然后我们来考虑左右端点l,r

如果颜色一致那么显然涂一种颜色即可
那么他就等于l+1,r和l,r-1的最小值
(为什么不能是l+1,r-1?)

如果颜色不一致
我们就要对区间节点进行枚举
最后取得l,r和l,k + k+1,r之间的小者
*/
# include <stdio.h>

int min(int a, int b) {
    return a > b ? b : a;
}

int solve(int l, int r, char * arr, int fun[51][51]) {
    if(fun[l][r] != 0) return fun[l][r];
    
    if(l == r) {
        return fun[l][r] = 1;
    }
    
    if(arr[l] == arr[r]) {
        fun[l][r] = min(solve(l+1, r, arr, fun), solve(l, r-1, arr, fun));
    }
    else {
        int minVal = 114514;
        for(int k = l; k < r; k++) {
            int temp = solve(l, k, arr, fun) + solve(k+1, r, arr, fun);
            if(temp < minVal) minVal = temp;
        }
        fun[l][r] = minVal;
    }
    return fun[l][r];
}

int main(void) {
    char target[51];
    int counter;
    char c;
    int fun[51][51] = {0};
    for(counter=0; (c = getchar()) != EOF && c != '\n'; counter++) {
        target[counter+1] = c;
    }
    
    int result = solve(1, counter, target, fun);
    printf("%d", result);
}