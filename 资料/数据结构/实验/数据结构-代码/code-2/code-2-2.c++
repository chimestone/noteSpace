#include <stdio.h>
#include <stdlib.h>
typedef int ElemType;
typedef  struct  Dnode{
    ElemType     data;
    struct  Dnode *prior,*next;
} *DuLinkList;

void adjust(DuLinkList L);

int main()
{
    DuLinkList L,p,tail;
    int n,i;
    L=tail=(DuLinkList)malloc(sizeof(struct Dnode));
    scanf("%d",&n);
    for(i=1;i<=n;i++)
    {
        p=(DuLinkList)malloc(sizeof(struct Dnode));
        scanf("%d",&p->data);
        tail->next=p;
        p->prior=tail;
        tail=p;
    }
    L->prior=tail;
    tail->next=L;
    adjust(L);
    p=L->next;
    while(p!=L)
    {
        printf(" %d",p->data);
        p=p->next;
    }
}
void adjust(DuLinkList L) {
    //考虑空链表或只有一个元素
    if (L == NULL || L->next->next == L) return;

    //考虑正常情况：odd 奇数;even 偶数
    DuLinkList odd, even, oddTail, evenTail,L1;
    DuLinkList p,p1;
    int num = 1;//奇偶计数器

    // 初始化奇偶链表的头尾节点
    odd = (DuLinkList)malloc(sizeof(struct Dnode));
    even = (DuLinkList)malloc(sizeof(struct Dnode));
    oddTail = odd;
    evenTail = even;

    for(p=L->next;p!=L;p=p->next,num++){
        if(num % 2 == 1) {
            p1=(DuLinkList)malloc(sizeof(struct Dnode));
            p1->data=p->data;
            oddTail->next=p1;
            p1->prior=oddTail;
            oddTail=p1;
        } else if(num % 2 == 0) {
            p1=(DuLinkList)malloc(sizeof(struct Dnode));
            p1->data=p->data;
            evenTail->prior=p1;
            p1->next=evenTail;
            evenTail=p1;
        }
    }
    odd->prior=even->prior;
    oddTail->next=evenTail;
    even->prior->next=odd;
    evenTail->prior=oddTail;

    L1=odd;
    for(p=L->next,p1=L1->next;p!=L;p=p->next,p1=p1->next){
        p->data=p1->data;
    }

    free(odd);
    free(even);

    return;
}