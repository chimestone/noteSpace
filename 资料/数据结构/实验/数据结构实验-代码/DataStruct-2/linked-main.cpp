#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#define TRUE 1
#define FALSE 0
#define OK 1
#define ERROR 0
#define INFEASIBLE -1
#define OVERFLOW -2

typedef int status;
typedef int ElemType; //数据元素类型定义

#define LIST_INIT_SIZE 100
#define LISTINCREMENT  10
typedef int ElemType;
typedef struct LNode{  //单链表（链式结构）结点的定义
    ElemType data;
    struct LNode *next;
    }LNode,*LinkList;
typedef struct LLNode{  //链表管理表结点定义
    LinkList L;
    struct LLNode *next;
    char name[32];
}LLNode,*LISTS;
//创建线性表
status InitList(LinkList &L)
// 线性表L不存在，构造一个空的线性表，返回OK，否则返回INFEASIBLE。
{
    if(L!=NULL)
        return INFEASIBLE; //已存在
    L=(LinkList)malloc(sizeof(LNode)); //分配空间
    if(L==NULL)
        return OVERFLOW;
    L->next=NULL;
    return OK;
}

//销毁线性表
status DestroyList(LinkList &L)
// 如果线性表L存在，销毁线性表L，释放数据元素的空间，返回OK，否则返回INFEASIBLE。
{
    if(L==NULL)
        return INFEASIBLE; //不存在
    LNode *p=L,*temp;
    if(L->next!=NULL)
    {
        p=L->next;
        while(p!=NULL)
        {
           temp=p->next;
           free(p);
           p=temp; 
        } //释放到只剩尾结点
    }
    free(L);
    L=NULL; //置空
    return OK;
}

//清空线性表
status ClearList(LinkList &L)
// 如果线性表L存在，删除线性表L中的所有元素，返回OK，否则返回INFEASIBLE。
{
    if(L==NULL)
        return INFEASIBLE; //不存在
    if(L->next==NULL)
        return OK;
    LNode *p=L->next,*temp;
    while(p!=NULL)
    {
        temp=p->next;
        free(p);
        p=temp;
    }
    L->next=NULL; //置空
    return OK;
}

//判空线性表
status ListEmpty(LinkList L)
// 如果线性表L存在，判断线性表L是否为空，空就返回TRUE，否则返回FALSE；如果线性表L不存在，返回INFEASIBLE。
{
    if(L==NULL)
        return INFEASIBLE; //表不存在
    if(L->next==NULL)
        return TRUE;
    return FALSE;
}

//求线性表长度
int ListLength(LinkList L)
// 如果线性表L存在，返回线性表L的长度，否则返回INFEASIBLE。
{
    if(L==NULL)
        return INFEASIBLE; //不存在
    int i=0;
    LNode *p;
    p=L->next;
    while(p!=NULL)
    {
        p=p->next;
        i++;
    } //遍历直到p为空
    return i;
}

//获取指定位置的元素
status GetElem(LinkList L,int i,ElemType &e)
// 如果线性表L存在，获取线性表L的第i个元素，保存在e中，返回OK；如果i不合法，返回ERROR；如果线性表L不存在，返回INFEASIBLE。
{
    if(L==NULL)
        return INFEASIBLE; //不存在
    int len=1;
    LNode *p=L->next;
    while(p!=NULL)
    {
        p=p->next;
        len++;
        if(len==i)
        {
            e=p->data;
            return OK;
        }
    } //遍历直到p为空
    return ERROR;
}

//获取指定元素的位置
status LocateElem(LinkList L,ElemType e)
// 如果线性表L存在，查找元素e在线性表L中的位置序号；如果e不存在，返回ERROR；当线性表L不存在时，返回INFEASIBLE。
{
    if(L==NULL)
        return INFEASIBLE; //不存在
    int len=1;
    LNode *p=L->next;
    while(p!=NULL)
    {
        if(p->data==e)
            return len; //找到e
        len++;
        p=p->next;
    }
    return ERROR; //e不存在
}

//获取前驱元素
status PriorElem(LinkList L,ElemType e,ElemType &pre)
// 如果线性表L存在，获取线性表L中元素e的前驱，保存在pre中，返回OK；如果没有前驱，返回ERROR；如果线性表L不存在，返回INFEASIBLE。
{
    if(L==NULL)
        return INFEASIBLE; //不存在
    LNode *p=L->next,*q;
    while(p!=NULL)
    {
        q=p->next;
        if(q!=NULL&&q->data==e) //找到e且位置合法
        {
            pre=p->data;
            return OK;
        }
        p=p->next;
    }
    return ERROR; //未找到e
}

//获取后继元素
status NextElem(LinkList L,ElemType e,ElemType &next)
// 如果线性表L存在，获取线性表L元素e的后继，保存在next中，返回OK；如果没有后继，返回ERROR；如果线性表L不存在，返回INFEASIBLE。
{
    if(L==NULL)
    return INFEASIBLE; //不存在
    LNode *p=L->next,*q;
    while(p!=NULL)
    {
        q=p->next;
        if(q!=NULL&&p->data==e) //找到e且位置合法
        {
            next=q->data;
            return OK;
        }
        p=p->next;
    }
    return ERROR; //未找到e
}

//插入元素
status ListInsert(LinkList &L,int i,ElemType e)
// 如果线性表L存在，将元素e插入到线性表L的第i个元素之前，返回OK；当插入位置不正确时，返回ERROR；如果线性表L不存在，返回INFEASIBLE。
{
    if(L==NULL)
        return INFEASIBLE; //不存在
    LNode *p=L->next,*q=L,*newbase;
    int len=1;
    while(p!=NULL)
    {
        if(len==i) //找到合法i
        {
        _INS:
            newbase=(LinkList)malloc(sizeof(LNode)); //申请空间
            if(newbase==NULL)
                return OVERFLOW;
            newbase->data=e;
            q->next=newbase;
            newbase->next=p;
            return OK;
        }
        p=p->next;
        q=q->next;
        len++;
    }
    if(len==i)
        goto _INS; //插入位置在最后一个
    return ERROR; //i不合法
}

//删除元素
status ListDelete(LinkList &L,int i,ElemType &e)
// 如果线性表L存在，删除线性表L的第i个元素，并保存在e中，返回OK；当删除位置不正确时，返回ERROR；如果线性表L不存在，返回INFEASIBLE。
{
    if(L==NULL)
        return INFEASIBLE; //不存在
    LNode *p=L->next,*prior=L;
    int len=1;
    while(p!=NULL)
    {
        if(len==i) //找到合法i
        {
            prior->next=p->next;
            e=p->data;
            free(p);
            return OK;
        }
        p=p->next;
        prior=prior->next;
        len++;
    }
    return ERROR; //未找到合法i
}

//遍历线性表
status ListTraverse(LinkList L)
// 如果线性表L存在，依次显示线性表中的元素，每个元素间空一格，返回OK；如果线性表L不存在，返回INFEASIBLE。
{
    if(L==NULL)
        return INFEASIBLE; //不存在
    LNode *p=L->next;
    while(p!=NULL)
    {
        printf("%d ",p->data);
        p=p->next;
    } //遍历输出直到p为空
    return OK;
}

//线性表写文件
status SaveList(LinkList L,char FileName[])
// 如果线性表L存在，将线性表L的的元素写到FileName文件中，返回OK，否则返回INFEASIBLE。
{
    if(L==NULL)
        return INFEASIBLE; //不存在
    FILE *fp;
    LNode *p=L->next;
    fp=fopen(FileName,"w");
    while(p!=NULL)
    {
        fprintf(fp,"%d ",p->data);
        p=p->next;
    }
    fprintf(fp,"\n");
    fclose(fp);
    return OK;
}

//线性表读文件
status LoadList(LinkList &L,char FileName[])
// 如果线性表L不存在，将FileName文件中的数据读入到线性表L中，返回OK，否则返回INFEASIBLE。
{
    if(L!=NULL)
        return INFEASIBLE; //已存在表 不能覆写
    L=(LinkList)malloc(sizeof(LNode));
    FILE *fp;
    LNode *p,*prior;
    ElemType e;
    fp=fopen(FileName,"r");
    for(prior=L;fscanf(fp,"%d",&e)==1;prior=prior->next)
    {
        p=(LinkList)malloc(sizeof(LNode));
        p->data=e;
        prior->next=p;
        p->next=NULL;
    } //顺序读取直到EOF
    fclose(fp);
    return OK;
}

//反转线性表
status ReverseList(LinkList &L)
{
    if(L==NULL)
        return INFEASIBLE; //不存在表
    LNode *p=L->next,*q;
    L->next=NULL;
    while(p!=NULL)
    {
        q=p->next;
        p->next=L->next;
        L->next=p;
        p=q;
    } //原地断裂后首插
    return OK;
}

//删除倒数第i个元素
status RemoveNthFromEnd(LinkList &L,int i,ElemType &e)
{
    if(L==NULL)
        return INFEASIBLE; //不存在表
    int l=0,n;
    LNode *p=L->next,*q;
    for(;p!=NULL;p=p->next,l++);
    if(i>l||i<1)
        return ERROR; //指定位置非法
    n=l-i+1;
    p=L->next;
    q=L;
    for(int j=1;j<n;j++,p=p->next,q=q->next);
    q->next=p->next;
    e=p->data;
    free(p);
    return OK;
}

//将线性表从小到大排序
status SortList(LinkList &L)
{
    if(L==NULL)
        return INFEASIBLE; //不存在表
    LNode *p=L->next,*q=L,*pp;
    int len;
    for(len=0;p!=NULL;p=p->next,len++);
    if(len<=1)
        return OK; //只有一个元素
    for(int i=0;i<len-1;i++)
    {    
        pp=L;
        p=L->next;
        q=p->next;    
        for(int j=0;j<len-i-1;j++,p=p->next,q=q->next,pp=pp->next)
            if(p->data>q->data)
            {
                pp->next=q;
                p->next=q->next;
                q->next=p;
                p=q;
                q=q->next;
            }
    }
    return OK;
}

//初始化线性表管理表
status InitLists(LISTS &LL)
{
    if(LL!=NULL)
        return INFEASIBLE; //已存在表
    LL=(LLNode*)malloc(sizeof(LLNode));
    if(LL==NULL)
        return OVERFLOW;
    LL->next=NULL;
    return OK;
}

//创建名称为name的线性表
status CreateList(LISTS &LL,char name[])
{
    if(LL==NULL)
        return INFEASIBLE; //不存在表
    LLNode *P=LL->next,*Q=LL;
    for(;P!=NULL;P=P->next,Q=Q->next)
        if(!strcmp(P->name,name))
            return ERROR; //名称已存在
    P=(LLNode*)malloc(sizeof(LLNode));
    if(P==NULL)
        return OVERFLOW;
    Q->next=P;
    P->next=NULL;
    strcpy(P->name,name);
    if(InitList(P->L)==OVERFLOW)
        return OVERFLOW;
    return OK;
}

//将当前线性表载入管理表
status WriteList(LISTS &LL,LinkList L,char name[])
{
    if(LL==NULL||L==NULL)
        return INFEASIBLE; //不存在表
    LLNode *P=LL->next,*Q=LL;
        for(;P!=NULL;P=P->next,Q=Q->next)
            if(!strcmp(P->name,name))
                return ERROR; //名称已存在
    P=(LLNode*)malloc(sizeof(LLNode));
    if(P==NULL)
        return OVERFLOW;
    Q->next=P;
    P->next=NULL;
    P->L=L;
    strcpy(P->name,name);
    return OK;
}

//将管理表中的线性表载入当前线性表
status ReadList(LISTS LL,LinkList &L,char name[])
{
    if(LL==NULL)
        return INFEASIBLE; //不存在
    LLNode *P=LL->next;
    int flag;
    for(flag=0;P!=NULL;P=P->next)
        if(P->L==L)
        {
            flag=1;
            break;
        }
    if(!flag)
        return INFEASIBLE; //当前线性表未保存 会造成丢失
    for(P=LL->next;P!=NULL;P=P->next)
        if(!strcmp(name,P->name))
        {
            L=P->L;
            return OK; //找到该表
        }
    return ERROR; //没找到该表
}

//列出当前管理表下的所有线性表
status ListLists(LISTS LL)
{
    if(LL==NULL)
        return INFEASIBLE; //不存在
    LLNode *P=LL->next;
    printf("当前线性表如下：\n");
    for(int i=1;P!=NULL;P=P->next,i++)
        printf("%d %s\n",i,P->name);
    return OK;
}

//定位指定线性表的位置
int LocateList(LISTS LL,char name[])
{
    if(LL==NULL)
        return INFEASIBLE; //不存在
    int i,flag=0;
    LLNode *P=LL->next;
    for(i=1;P!=NULL;P=P->next,i++)
        if(!strcmp(P->name,name))
        {
            flag=i;
            break;
        }
    return flag;
}

//删除指定线性表
status RemoveList(LISTS &LL,LinkList L,char name[])
{
    if(LL==NULL)
        return INFEASIBLE; //不存在
    LLNode *P=LL->next,*Q=LL;
    LNode *p,*q;
        for(;P!=NULL;P=P->next,Q=Q->next)
            if(!strcmp(P->name,name))
            {
                if(P->L!=L) //如果当前使用的线性表不是要被移除的线性表 则释放空间
                {
                    p=P->L->next;
                    while(p!=NULL)
                    {
                        q=p->next;
                        free(p);
                        p=q;
                    }
                    free(P->L);
                    P->L=NULL;
                }
                Q->next=P->next;
                free(P);
                return OK;
            }
    return ERROR; //没找到
}

//销毁管理表
status DeleteLists(LISTS &LL,LinkList L)
{
    if(LL==NULL)
        return INFEASIBLE; //不存在
        LLNode *P=LL->next,*Q;
        LNode *p,*q;
        while(P!=NULL)
        {
           if(P->L!=L) //如果当前使用的线性表不是要被移除的线性表 则释放空间
            {
                p=P->L->next;
                while(p!=NULL)
                {
                    q=p->next;
                    free(p);
                    p=q;
                }
                free(P->L);
            }
            Q=P->next;
            free(P);
            P=Q;
        }
        free(LL);
        LL=NULL;
        return OK;
}
int main()
{
    LinkList L=NULL; //新建线性表
    LISTS LL=NULL; //新建线性表管理表
    LNode *temp;
    int op=1;
    char name[31],FileName[1024];
    status flag;
    int i,len,out,k,num;
    ElemType e,en,ep; //变量定义

_LinkList:
op=1;
while(op)
{
    system("cls");	
    system("clear");
    printf("\n\n");
    printf("\t单线性表操作菜单 \n");
    printf("-------------------------------------------------\n");
    printf("\t1. 创建表\t10. 插入表\n");
    printf("\t2. 销毁表\t11. 删除表\n");
    printf("\t3. 清空表\t12. 遍历表\n");
    printf("\t4. 表判空\t13. 保存表\n");
    printf("\t5. 表长度\t14. 加载表\n");
    printf("\t6. 获取元素\t15. 反转表\n");
    printf("\t7. 定位元素\t16. 删除倒数结点\n");
    printf("\t8. 前驱元素\t17. 表排序\n");
    printf("\t9. 后驱元素\t18. 多列表\n");
    printf("\t0. 退出\n");
    printf("-------------------------------------------------\n");
    printf("    请选择你的操作[0~18]:");
    scanf("%d",&op);
    switch(op)
        {
            case 1:
                if(InitList(L)==OK) 
                    printf("线性表创建成功！\n");
                else
                    printf("线性表创建失败！线性表已存在！\n");
                printf("按回车继续！\n");
                getchar();getchar();
                break;
            case 2:
                if(DestroyList(L)==OK) 
                    printf("线性表销毁成功！\n");
                else
                    printf("线性表销毁失败！线性表不存在！\n");
                printf("按回车继续！\n");
                getchar();getchar();
                break;
            case 3:
                if(ClearList(L)==OK) 
                    printf("线性表清空成功！\n");
                else
                    printf("线性表清空失败！线性表不存在！\n");
                printf("按回车继续！\n");
                getchar();getchar();
                break;
            case 4:
                flag=ListEmpty(L);
                if(flag)
                    printf("线性表为空！\n");
                else if(!flag)
                    printf("线性表不为空！\n");
                else
                    printf("线性表判空失败！线性表不存在！\n");
                printf("按回车继续！\n");
                getchar();getchar();
                break;
            case 5:
                len=ListLength(L);
                if(len!=INFEASIBLE)
                    printf("线性表长度为%d！\n",len);
                else
                    printf("读取线性表长度失败！线性表不存在！\n");
                printf("按回车继续！\n");
                getchar();getchar();
                break;
            case 6:
                printf("请输入要获取元素的逻辑位置i！\n");
                scanf("%d",&i);
                flag=GetElem(L,i,e);
                if(flag==INFEASIBLE)
                    printf("获取元素失败！线性表不存在！\n");
                else if(flag==ERROR)
                    printf("获取元素失败！输入的i非法！\n");
                else
                    printf("第%d个元素是%d！\n",i,e);
                printf("按回车继续！\n");
                getchar();getchar();
                break;
            case 7:
                printf("请输入需要定位的元素e！\n");
                scanf("%d",&e);
                i=LocateElem(L,e);
                if(i==INFEASIBLE)
                    printf("定位失败！线性表不存在！\n");
                else if(i==ERROR)
                    printf("定位失败！该元素不存在！\n");
                else
                    printf("该数字在第%d位！\n",i);
                printf("按回车继续！\n");
                getchar();getchar();
                break;
            case 8:
                printf("请输入要获取前驱的元素e！\n");
                scanf("%d",&e);
                flag=PriorElem(L,e,ep);
                if(flag==INFEASIBLE)
                    printf("获取失败！线性表不存在！\n");
                else if(flag==ERROR)
                {
                    if(e==L->next->data)
                        printf("这个元素没有前驱！\n");
                    else
                        printf("获取失败！未找到该元素！\n");
                }
                else
                    printf("前驱是%d！\n",ep);
                printf("按回车继续！\n");
                getchar();getchar();
                break;
            case 9:
                printf("请输入要获取后继的元素e！\n");
                scanf("%d",&e);
                flag=NextElem(L,e,en);
                if(flag==INFEASIBLE)
                    printf("获取失败！线性表不存在！\n");
                else if(flag==ERROR)
                {   
                    for(temp=L;temp->next!=NULL;temp=temp->next);
                    
                    if(L->next!=NULL&&temp->data==e)
                        printf("这个元素没有后继！\n");
                    else
                        printf("获取失败！未找到该元素！\n");
                }
                else
                    printf("后继是%d！\n",en);
                printf("按回车继续！\n");    
                getchar();getchar();
                break;
            case 10:
                printf("请输入要插入的位置i和插入的元素e（中间用空格隔开）！\n");
                scanf("%d %d",&i,&e);
                flag=ListInsert(L,i,e);
                if(flag==INFEASIBLE)
                    printf("插入失败！线性表不存在！\n");
                else if(flag==ERROR)
                    printf("插入失败！插入位置非法！\n");
                else if(flag==OVERFLOW)
                    printf("插入失败！线性表已满且扩容失败！\n");
                else
                    printf("插入成功！\n");
                printf("按回车继续！\n");  
             getchar();getchar();
             break;
            case 11:
                printf("请输入要删除元素的逻辑位置i！\n");
                scanf("%d",&i);
                flag=ListDelete(L,i,e);
                if(flag==INFEASIBLE)
                    printf("删除失败！线性表不存在！\n");
                else if(flag==ERROR)
                    printf("删除失败！删除位置非法！\n");
                else
                    printf("删除成功！删除的元素是%d！\n",e);
                printf("按回车继续！\n");    
                getchar();getchar();
                break;
            case 12:  
                if(!ListTraverse(L)) 
                    printf("线性表是空表！\n");
                else
                    printf("\n");
                printf("按回车继续！\n"); 
                getchar();getchar();
                break;
            case 13:
                printf("请输入保存文件的地址：\n");
                scanf("%s",FileName);
                flag=SaveList(L,FileName);
                if(flag==INFEASIBLE)
                    printf("保存错误！线性表不存在！\n");
                else
                    printf("保存成功！请查看output.dat！\n");
                printf("按回车继续！\n"); 
                getchar();getchar();
                break;
            case 14:
                printf("请输入读取文件的地址：\n");
                scanf("%s",FileName);
                flag=LoadList(L,FileName);
                if(flag==INFEASIBLE)
                    printf("读取错误！线性表已存在！\n");
                else if(flag==OVERFLOW)
                    printf("读取错误！扩容失败！\n");
                else if(flag==ERROR)
                    perror("读取错误！打开文件失败");
                else
                    printf("读取成功！\n");
                printf("按回车继续！\n"); 
                getchar();getchar();
                break;
            case 15:
                if(ReverseList(L)==INFEASIBLE)
                    printf("反转失败！线性表不存在！\n");
                else
                    printf("反转成功！\n");
                printf("按回车继续！\n"); 
                getchar();getchar();
                break;
            case 16:
                printf("请输入要删除的倒数第i个元素的位置i！\n");
                scanf("%d",&i);
                flag=RemoveNthFromEnd(L,i,e);
                if(flag==INFEASIBLE)
                    printf("删除失败！线性表不存在！\n");
                else if(flag==ERROR)
                    printf("删除失败！位置i非法！\n");
                else
                    printf("删除成功！被删除的元素是%d！\n",e);
                printf("按回车继续！\n"); 
                getchar();getchar();
                break;
            case 17:
                if(SortList(L)==INFEASIBLE)
                    printf("排序失败！线性表不存在！\n");
                else
                    printf("排序成功！\n");
                printf("按回车继续！\n"); 
                getchar();getchar();
                break;
            case 18:
                goto _LISTS;
            case 0:
                break;
            default:
                printf("输入错误！请重新输入！\n");
                printf("按回车继续！\n"); 
                getchar();getchar();
                break;
        }
    }
_LISTS:
    while(op)
    {
        system("cls");
        system("clear");
        printf("\n\n");
        printf("\t线性表管理表操作菜单 \n");
        printf("-------------------------------------------------\n");
        printf("\t1. 初始化\t5. 表列表\n");
        printf("\t2. 创建表\t6. 载入表\n");
        printf("\t3. 写入表\t7. 删除表\n");
        printf("\t4. 读入表\t8. 销毁表\n");
        printf("\t0. 退出\n");
        printf("-------------------------------------------------\n");
        printf("    请选择你的操作[0~8]:");
        scanf("%d",&op);
        switch(op)
        {
            case 1:
                flag=InitLists(LL);
                if(flag==INFEASIBLE)
                    printf("初始化失败！已存在管理表不可操作！\n");
                else if(flag==OVERFLOW)
                    printf("初始化失败！申请空间失败！");
                else
                    printf("初始化成功！");
                printf("按回车继续！\n"); 
                getchar();getchar();
                break;
            case 2:
                printf("请输入线性表名称！\n");
                scanf("%s",name);
                flag=CreateList(LL,name);
                if(flag==INFEASIBLE)
                    printf("创建失败！管理表不存在！\n");
                else if(flag==OVERFLOW)
                    printf("创建失败！扩容失败！\n");
                else if(flag==ERROR)
                    printf("创建失败！名称已存在！\n");
                else
                    printf("创建成功！\n");
                printf("按回车继续！\n"); 
                getchar();getchar();
                break;
            case 3:
                printf("请输入线性表的名称！\n");
                scanf("%s",name);
                flag=WriteList(LL,L,name);
                if(flag==INFEASIBLE)
                {
                    if(LL==NULL)
                        printf("写入失败！管理表不存在！\n");
                    else
                        printf("写入失败！当前线性表不存在！\n");
                }
                else if(flag==ERROR)
                    printf("写入失败！名称已存在！\n");
                else if(flag==OVERFLOW)
                    printf("写入失败！扩容失败！\n");
                else
                    printf("写入成功！\n");
                printf("按回车继续！\n"); 
                getchar();getchar();
                break;
            case 4:
                printf("请输入要载入线性表的名称！\n");
                scanf("%s",name);
                flag=ReadList(LL,L,name);
                if(flag==INFEASIBLE)
                {    
                    if(LL==NULL)
                        printf("载入失败！管理表不存在！\n");
                    else
                        printf("当前线性表尚未保存！不可覆写！\n");
                }
                else if(flag==ERROR)
                    printf("载入失败！该名称线性表不存在！\n");
                else
                    printf("载入成功！\n");
                printf("按回车继续！\n"); 
                getchar();getchar();
                break;
            case 5:
                if(ListLists(LL)==INFEASIBLE)
                    printf("列举失败！管理表不存在！\n");
                printf("按回车继续！\n"); 
                getchar();getchar();
                break;
            case 6:
                printf("请输入要定位的线性表名称！\n");
                scanf("%s",name);
                i=LocateList(LL,name);
                if(i==INFEASIBLE)
                    printf("定位失败！管理表不存在！\n");
                else if(!i)
                    printf("定位失败！该名称线性表不存在！\n");
                else
                    printf("这是第%d个线性表！\n",i);
                printf("按回车继续！\n"); 
                getchar();getchar();
                break;
            case 7:
                printf("请输入要删除的线性表名称！\n");
                scanf("%s",name);
                flag=RemoveList(LL,L,name);
                if(flag==INFEASIBLE)
                    printf("删除失败！管理表不存在！\n");
                else if(flag==ERROR)
                    printf("删除失败！该名称线性表不存在！\n");
                else
                    printf("删除成功！\n");
                printf("按回车继续！\n"); 
                getchar();getchar();
                break;
            case 8:
                if(DeleteLists(LL,L)==INFEASIBLE)
                    printf("销毁失败！管理表不存在！\n");
                else
                    printf("销毁成功！\n");
                printf("按回车继续！\n"); 
                getchar();getchar();
                break;
            case 0:
                goto _LinkList;
                break;
            default:
                printf("输入错误！请重新输入！\n");
                printf("按回车继续！\n"); 
                getchar();getchar();
                break;
        }
    }
    printf("欢迎下次再使用本系统！\n");
    LNode *p,*q;
    LLNode *P,*Q;
    if(LL!=NULL)
    {
        P=LL->next;
        while(P!=NULL)
        {
            p=P->L->next;
            while(p!=NULL)
            {
                q=p->next;
                free(p);
                p=q;
            }
            free(P->L);
            P->L=NULL;
            Q=P->next;
            free(P);
            P=Q;
        }
        free(LL);
        LL=NULL;
    }
    if(L!=NULL)
    {
        p=L->next;
        while(p!=NULL)
            {
                q=p->next;
                free(p);
                p=q;
            }
        free(L);
        L=NULL;
    }
    printf("按回车继续！\n"); 
    getchar();getchar();
    return 0;
}
