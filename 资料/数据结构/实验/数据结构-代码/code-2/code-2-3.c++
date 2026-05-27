#include<stdio.h>
#include<stdlib.h>
#define LIST_INIT_SIZE 5000
#define LISTINCREMENT 10
typedef int ElemType;
typedef struct
   { ElemType *elem;
     int length;
     int listsize;
    } SqList;
void TriSqList(SqList &A, SqList B, SqList C) ;
int main()
{
    SqList L[3];
    ElemType x;
    int i;
    for(i=0;i<3;i++)
    {
    	L[i].elem=(ElemType *)malloc(sizeof(ElemType)*LIST_INIT_SIZE);
    	L[i].length=0;
    	L[i].listsize=LIST_INIT_SIZE;
    	scanf("%d",&x);
    	while (x)
    	{
        	L[i].elem[L[i].length++]=x;
        	scanf("%d",&x);
    	}
    }
    TriSqList(L[0],L[1],L[2]);
    for(i=0;i<L[0].length;i++)
        printf(" %d",L[0].elem[i]);
	return 0;
}

void TriSqList(SqList &A, SqList B, SqList C) {
    // 创建哈希表存储B和C中的元素
    bool *hashB = (bool *)calloc(LIST_INIT_SIZE, sizeof(bool));
    bool *hashC = (bool *)calloc(LIST_INIT_SIZE, sizeof(bool));

    // 将B表中的元素存入哈希表
    for (int i = 0; i < B.length; i++) {
        hashB[B.elem[i]] = true;
    }

    // 将C表中的元素存入哈希表
    for (int i = 0; i < C.length; i++) {
        hashC[C.elem[i]] = true;
    }

    // 遍历A表，删除同时在B和C中的元素
    int j = 0; // 用于记录新数组的索引
    for (int i = 0; i < A.length; i++) {
        if (hashB[A.elem[i]] && hashC[A.elem[i]]) {
            // 如果元素同时存在于B和C中，跳过
            continue;
        } else {
            // 否则，保留该元素
            A.elem[j++] = A.elem[i];
        }
    }

    A.length = j;

    free(hashB);
    free(hashC);
}