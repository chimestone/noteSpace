# include <iostream>
# include <vector>
using namespace std;

vector<int> parent;

int find(int x)
{
    if(x != parent[x])
    {
        parent[x] = find(parent[x]);
    }
    return parent[x];
}

void merge(int x, int y)
{
    int fx = find(x);
    int fy = find(y);

    parent[fx] = fy;

}

int main(void)
{
    int n,k;
    cin >> n >> k;
    parent.resize(3*n);

    for(int i=0; i<3*n; i++)
    {
        parent[i] = i;
    }

    int ans = 0;
    for(int i=0; i<k; i++)
    {
        int choice,x,y;
        cin >> choice >> x >> y;

        if(x > n || y > n) ans++;
        else
        {
            if(choice == 1)
            {
                if(find(x+n)==find(y)||find(x+2*n)==find(y))
                {
                    ans++;
                }
                else
                {
                    merge(x,y);
                    merge(x+n,y+n);
                    merge(x+2*n,y+2*n);
                }
            }
            else
            {
                if(x==y||find(x)==find(y)||find(x+2*n)==find(y))
                {
                    ans ++;
                }
                else
                {
                    merge(x,y+2*n);
                    merge(x+n,y);
                    merge(x+2*n,y+n);
                }
            }
        }
    }

    cout << ans;
    return 0;
}