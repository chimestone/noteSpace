# include <iostream>
using namespace std;

long long tree[5000005];
int n;

int lowbit(int x) {
    return x & (-x);
}

void update(int x, long long k) {
    for(int i=x; i<=n; i+=lowbit(i)) {
        tree[i] += k;
    }
}

long long query(int x) {
    long long result = 0;
    for(int i=x; i>0; i-=lowbit(i)) {
        result += tree[i];
    }
    return result;
}

void add(int l, int r, int k) {
    update(l,k);
    if(r+1 <= n) {
        update(r+1,-k);
    }
}

int main(void) {
    int m;
    cin >> n >> m;

    for(int i=0; i<n; i++) {
        tree[i]=0;
    }

    long long pre=0,cur;
    for(int i=1; i<=n; i++) {
        cin >> cur;
        update(i,cur-pre);
        pre = cur;
    }

    for(int i=0; i<m; i++) {
        int flag;
        cin >> flag;

        switch (flag){
            case 1:
                int left,right,kVal;
                cin >> left >> right >> kVal;
                add(left,right,kVal);
                break;
            case 2:
                int pos;
                cin >> pos;
                cout << query(pos) << endl;
                break;
            default:
                cout << "Error" << endl;
        }
    }

    return 0;
}