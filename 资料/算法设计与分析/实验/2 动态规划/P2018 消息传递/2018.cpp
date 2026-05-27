# include <iostream>
# include <cstring>
# include <algorithm>

using namespace std;

int cnt,n,curans = 0xfffff,f[2002],ans[2002],h[2002];
bool v[2002];

struct edge {
    int n,t;
}e[2002];

void add(int a, int b) {
    e[++cnt].t = b;
    e[cnt].n = h[a];
    h[a] = cnt;
}

void dp(int x) {
    int d[2002] = {0},y;
    v[x] = true;

    for(int i=h[x]; i; i=e[i].n) {
        y=e[i].t;
        if(!v[y]) {
            dp(y);
            d[++d[0]] = f[y];
        }
    }

    sort(d+1,d+d[0]+1);

    for(int i=1; i<= d[0]; i++){
        f[x] = max(f[x],d[i]+d[0]-i+1);
    }
}

int main(void) {
    cin >> n;
    for(int i=1; i<n; i++) {
        int x;
        cin >> x;
        add(i+1,x);
        add(x,i+1);
    }

    for(int i=1; i<=n; i++) {
        memset(f,0,sizeof(f));
        memset(v,0,sizeof(v));
        dp(i);

        if(curans > f[i]) {
            curans = f[i];
            ans[0] = 0;
            ans[++ans[0]] = i;
        } else if(curans == f[i]) {
            ans[++ans[0]] = i;
        }
    }

    cout << curans+1 <<endl;
    for(int i=0; i<ans[0]; i++) {
        cout << ans[i+1] << ' ';
    }
    return 0;
}