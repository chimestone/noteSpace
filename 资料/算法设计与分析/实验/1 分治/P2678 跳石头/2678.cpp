/*
最大距离 l
石头数量 n
挪走数量 m
石头值 arr[i]

result d 最小距离
我们取定一个d 那么应当满足：
若石头间距大于等于d 直接走过去
若石头间距小于d 将石头挪走再次判断
最终挪动石头次数应小于等于m
应当保证存在至少一个等于d的值 保证是最短跳跃距离
如果说d满足,那么大于等于d的必然满足
*/
# include <stdio.h>
# include <stdlib.h>

int * arr;
int l,n,m;

int isMeet(int d)
{
    int moved = 0;
    int last = 0;

    for (int i = 0; i <= n; i++)
    {
        if (arr[i] - last < d)
        {
            moved++;
            if (moved > m)
                return 0;
        }
        else
        {
            last = arr[i];
        }
    }
    return 1;
}

int calMax(int left, int right) {
    int result = 0;
    while(left <= right) {
        int mid = (left + right) / 2;
        if(isMeet(mid)) {
            result = mid;
            left = mid + 1;
        } else {
            right = mid - 1;
        }
    }
    return result;
}

int main(void) {
    scanf("%d %d %d",&l,&n,&m);
    arr = (int *)malloc(sizeof(int) * (n+1));
    for(int i=0; i<n; i++) {
        scanf("%d",&arr[i]);
    }
    arr[n] = l;

    int result = calMax(1,l);
    printf("%d",result);

    free(arr);
}