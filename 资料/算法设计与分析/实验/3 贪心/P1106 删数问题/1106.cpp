/*
输入
n   一个正整数
k   删去数字的数量

输出 按照原次序删去的最小新数
注意不要输出前导零

从前往后 找到递减区间则进行删除
直到删除过程结束
*/
#include <stdio.h>
#include <string.h>

int main(void)
{
    char num[100];
    int k;
    scanf("%s %d", num, &k);
    
    int len = strlen(num);
    int toDelete = k;
    
    for (int i = 0; i < len - 1 && toDelete > 0; i++) {
        if (num[i] > num[i + 1]) {
            for (int j = i; j < len - 1; j++) {
                num[j] = num[j + 1];
            }
            len--;
            toDelete--;
            i = -1;
        }
    }
    
    len -= toDelete;
    
    int start = 0;
    while (start < len && num[start] == '0') {
        start++;
    }

    if (start == len) {
        printf("0");
    } else {
        for (int i = start; i < len; i++) {
            printf("%c", num[i]);
        }
    }
    
    return 0;
}