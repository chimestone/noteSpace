#include<stdio.h>
#include<stdlib.h>
typedef int ElemType;
typedef struct node
   { ElemType data;
     struct node *next;
    } NODE,*LinkList;
void TriLinkList(LinkList  A,LinkList  B,LinkList  C);
int main()
{
    LinkList L[3],p;
    ElemType x;
    int i;
    for(i=0;i<3;i++)
    {
    	L[i]=p=(NODE *)malloc(sizeof(NODE));
    	scanf("%d",&x);
    	while (x)
    	{
				p->next=(NODE *)malloc(sizeof(NODE));
				p=p->next;
				p->data=x;
				scanf("%d",&x);
    	}
      p->next=NULL;
    }
    TriLinkList(L[0],L[1],L[2]);
    for(p=L[0]->next;p;p=p->next)
        printf(" %d",p->data);
	return 0;
}
void TriLinkList(LinkList A, LinkList B, LinkList C)
{
// 创建交集链表D
LinkList D = (LinkList)malloc(sizeof(NODE));
D->next = NULL;
NODE *pD_tail = D;

NODE *pB = B->next;
NODE *pC = C->next;

// 构建B和C的交集D
while (pB && pC) {
    if (pB->data == pC->data) {
        // 创建新节点并添加到D
        NODE *newNode = (NODE*)malloc(sizeof(NODE));
        newNode->data = pB->data;
        newNode->next = NULL;
        pD_tail->next = newNode;
        pD_tail = newNode;
        pB = pB->next;
        pC = pC->next;
    } else if (pB->data < pC->data) {
        pB = pB->next;
    } else {
        pC = pC->next;
    }
}

// 删除A中存在于D的元素
NODE *pre = A;
NODE *cur = A->next;
NODE *pD = D->next;

while (cur && pD) {
    if (cur->data == pD->data) {
        // 删除当前节点
        pre->next = cur->next;
        NODE *temp = cur;
        cur = cur->next;
        free(temp);
        pD = pD->next;
    } else if (cur->data < pD->data) {
        pre = cur;
        cur = cur->next;
    } else {
        pD = pD->next;
    }
}

// 释放D链表的内存
while (D) {
    NODE *temp = D;
    D = D->next;
    free(temp);
}
}