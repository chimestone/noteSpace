# include <iostream>
using namespace std;

long long powerOfTwo(int k) {//2的k次幂
    long long result =1;
    for(int i=0; i<k; i++) {
        result *= 2;
    }
    return result;
}

void solve(long long x, long long y, long long a, long long b, long long len) {//核心函数
    if(len == 1) return;

    if((x - a < len / 2)&&(y - b < len / 2)) {
        cout << a + len / 2 <<' '<< b + len /2 <<' '<< 1 << endl;
        solve(x, y, a, b, len / 2);
        solve(a + len / 2 - 1, b + len / 2, a, b + len / 2, len / 2);
        solve(a + len / 2, b + len / 2 - 1, a + len / 2, b, len / 2);
        solve(a + len / 2, b + len / 2, a + len / 2, b + len / 2, len / 2);
    } else if((x - a < len / 2)&&(y - b >= len / 2)) {
        cout << a + len / 2  <<' '<< b + len /2 - 1<<' '<< 2 << endl;
        solve(a + len / 2 - 1, b + len / 2 - 1, a, b, len / 2);
        solve(x, y, a, b + len / 2, len / 2);
        solve(a + len / 2, b + len / 2 - 1, a + len / 2, b, len / 2);
        solve(a + len / 2, b + len / 2, a + len / 2, b + len / 2, len / 2);
    }
    else if ((x - a >= len / 2) && (y - b < len / 2))
    {
        cout << a + len / 2 - 1 << ' ' << b + len / 2  <<' '<< 3 << endl;
        solve(a + len / 2 - 1, b + len / 2 - 1, a, b, len / 2);
        solve(a + len / 2 - 1, b + len / 2, a, b + len / 2, len / 2);
        solve(x, y, a + len / 2, b, len / 2);
        solve(a + len / 2, b + len / 2, a + len / 2, b + len / 2, len / 2);
    } else {
        cout << a + len / 2 - 1 << ' ' << b + len / 2 - 1 <<' '<< 4 << endl;
        solve(a + len / 2 - 1, b + len / 2 - 1, a, b, len / 2);
        solve(a + len / 2 - 1, b + len / 2, a, b + len / 2, len / 2);
        solve(a + len / 2, b + len / 2 - 1, a + len / 2, b, len / 2);
        solve(x, y, a + len / 2, b + len / 2, len / 2);
    }
}

int main(void) {
    int k;
    cin >> k;
    long long x, y;
    cin >> x >> y;
    long long len = powerOfTwo(k);
    solve(x,y,1,1,len);
    return 0;
}