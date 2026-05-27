#include<stdio.h>
#include<stdlib.h>
#define LIST_INIT_SIZE 20
#define LISTINCREMENT 10
typedef int ElemType;
typedef struct
   { ElemType *elem;    //元素类型 指针elem
     int length;        //线性表长度
     int listsize;      //线性表最大长度
    } SqList;
int insert(SqList &L,ElemType x);
int main()
{
    SqList L;
    ElemType x;
    int i;
    L.elem=(ElemType *)malloc(sizeof(ElemType)*LIST_INIT_SIZE);
    L.length=0;
    L.listsize=LIST_INIT_SIZE;
    scanf("%d",&x);
    while (x)
    {
        L.elem[L.length++]=x;
        scanf("%d",&x);
    }
    scanf("%d",&x);
    insert(L,x);
    for(i=0;i<L.length;i++)
        printf("%8d",L.elem[i]);
    if (L.length>L.listsize) printf("\n程序未处理溢出");

	return 0;
}

int insert(SqList &L,ElemType x)
{
    int flag=0;
    if(L.length==0) {//原来无元素
        L.length++;
        L.elem[0]=x;
    }
    else {
        if(L.elem[0]>=x) {//首元素大于等于x
            for(int i=L.length;i>0;i--) L.elem[i]=L.elem[i-1];
            L.elem[0]=x;
            L.length++;
        }
        else if(L.elem[L.length]<=x) {//尾元素小于等于x
            L.length++;
            L.elem[L.length-1]=x;
        }
        else {//中间插入
            for(int i=0;L.elem[i]<=x;i++) flag++;
            for(int i=L.length;i>flag;i--) L.elem[i]=L.elem[i-1];
            L.elem[flag]=x;
            L.length++;
        }
    }
    return 0;
}

//问题一：length的值没有改变 已解决
//问题二：中间插入有问题
//问题三：尾元素没有考虑 已解决