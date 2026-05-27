//头文件和定义
#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <windows.h>

#define TRUE 1
#define FALSE 0
#define OK 1
#define ERROR 0
#define INFEASIBLE -1
#define OVERFLOW -2

typedef int status; 
typedef int ElemType;

#define LIST_INIT_SIZE 100
#define LISTINCREMENT  10

typedef struct{
	ElemType * elem;
	int length;
	int listsize;
}SqList;

typedef struct{
	struct { char name[30];
			  SqList L;	
	 } elem[10];
	 int length;
	 int listsize;
}LISTS;

//众多函数的预定义
status InitList(SqList& L);
status DestroyList(SqList& L);
status ClearList(SqList&L);
status ListEmpty(SqList L);
int ListLength(SqList L);
status GetElem(SqList L,int i,ElemType& e);
status LocateElem(SqList L,ElemType e);
status PriorElem(SqList L,ElemType cur,ElemType&pre_e);
status NextElem(SqList L,ElemType cur,ElemType&next_e);
status ListInsert(SqList&L,int i,ElemType e);
status ListDelete(SqList&L,int i,ElemType& e);
status ListTraverse(SqList L);
status AddList(LISTS &Lists, char ListName[]);
int LocateList(LISTS Lists,char ListName[]);
status RemoveList(LISTS &Lists,char ListName[]);
status SortList(SqList& L);
status MaxSubArray(SqList& L);
status SubArrayNum(SqList& L,int k);
status SaveList(SqList L, char FileName[]);
status LoadList(SqList &L, char FileName[]);

//主函数
int main(void){
    int op=1,n,e; char s[30]; LISTS Lists;
    Lists.length=0;
    SetConsoleOutputCP(CP_UTF8);
while(op){
	system("cls");	printf("\n\n");
	printf("              线性表数据结构实验菜单 \n");
	printf("-------------------------------------------------\n");
	printf("    	  1.创建线性表     10.插入元素\n");
    printf("          2.销毁线性表     11.删除元素\n");
    printf("          3.清空线性表     12.遍历线性表\n");
	printf("    	  4.线性表判空     13.最大连续子数组和\n");
	printf("    	  5.线性表长度     14.和为k的连续子数组\n");
	printf("    	  6.获取元素       15.线性表排序\n");
	printf("    	  7.定位元素       16.定位线性表\n");
	printf("    	  8.前驱元素       17.文件操作\n");
    printf("          9.后驱元素       18.遍历所有线性表\n");
	printf("    	  0. 退出\n");
	printf("-------------------------------------------------\n");
	printf("请选择你的操作：[0~18]:\n");
	scanf("%d",&op);
    switch(op){
		case 1:
			printf("请输入你希望新增的线性表个数：\n");
            scanf("%d",&n);
            for (int i=0; i<n; i++) {
                printf("请输入第%d个线性表的名称:\n",i+1);
                scanf("%s",s);
                if(AddList(Lists,s)==ERROR) {
                    printf("线性表创建失败！\n");
                    break;
                }
                printf("请输入这个线性表的元素,元素之间以空格分隔,以0作结:\n");
                scanf("%d",&e);
                while (e){
                    ListInsert(Lists.elem[Lists.length-1].L,Lists.elem[Lists.length-1].L.length+1,e);
                    scanf("%d",&e);
                }
                printf("线性表创建成功！\n");
            }
			getchar();getchar();
			break;
	    case 2:
            printf("请输入你想要销毁的线性表的名称:\n");
            scanf("%s",s);
            switch (RemoveList(Lists,s)) {
                case INFEASIBLE:
                    printf("线性表不存在！\n");
                    break;
                case ERROR:
                    printf("未找到该线性表！\n");
                    break;
                case OK:
                    Lists.length--;
                    printf("该线性表销毁成功！\n");
                    break;
            }
			getchar();getchar();
			break;
	    case 3:
	   		printf("请输入你想要清空的线性表的名称：\n");
            scanf("%s",s);
            if (LocateList(Lists,s)==0) printf("未找到该线性表！\n");
            else {
                if(ClearList(Lists.elem[LocateList(Lists,s)-1].L)==INFEASIBLE) printf("线性表清空失败！\n");
                else printf("线性表清空成功！\n");
            }
			getchar();getchar();
			break;
	    case 4:
            printf("请输入你希望判空的线性表的名称：\n");
            scanf("%s",s);
            if (LocateList(Lists,s)==0) printf("未找到该线性表！\n");
            else {
			    switch (ListEmpty(Lists.elem[LocateList(Lists,s)-1].L)) {
			        case TRUE: 
					    printf("线性表为空！\n"); break;
				    case FALSE:
					    printf("线性表不为空！\n"); break;
				    case INFEASIBLE:
					    printf("线性表不存在！\n"); break;
			    }   
            }
		 	getchar();getchar();
		 	break;
	    case 5:
            printf("请输入你希望求长度的线性表的名称：\n");
            scanf("%s",s);
            if (LocateList(Lists,s)==0) printf("未找到该线性表！\n");
			else {
                if(ListLength(Lists.elem[LocateList(Lists,s)-1].L)==INFEASIBLE) printf("线性表不存在！\n");
			    else printf("线性表的长度是%d!\n",ListLength(Lists.elem[LocateList(Lists,s)-1].L));
            }
		 	getchar();getchar();
		 	break;
	    case 6:
            printf("请输入你想要获取元素的线性表的名称：\n");
            scanf("%s",s);
            if (LocateList(Lists,s)==0) printf("未找到该线性表！\n");
			else {
                int getelem,seq;
			    printf("输入你想要获取的线性表元素的序号：\n");
			    scanf("%d",&seq);
			    switch (GetElem(Lists.elem[LocateList(Lists,s)-1].L,seq,getelem)) {
				    case INFEASIBLE: printf("线性表不存在！\n"); break;
				    case ERROR: printf("输入不合法！\n"); break;
				    case OK: printf("线性表第%d位的元素是%d!\n",seq,getelem); break;
			    }
            }
			getchar();getchar();
			break;
	    case 7:
            printf("请输入你希望定位元素的线性表的名称：\n");
            scanf("%s",s);
            if (LocateList(Lists,s)==0) printf("未找到该线性表！\n");
            else {
			    ElemType find_elem;
			    printf("请输入你要查找的元素：\n");
			    scanf("%d",&find_elem);
			    if (LocateElem(Lists.elem[LocateList(Lists,s)-1].L,find_elem)==INFEASIBLE) printf("线性表不存在!\n");
			    if (LocateElem(Lists.elem[LocateList(Lists,s)-1].L,find_elem)>0) printf("%d元素在线性表的第%d位!\n",find_elem,LocateElem(Lists.elem[LocateList(Lists,s)-1].L,find_elem));
			    if (LocateElem(Lists.elem[LocateList(Lists,s)-1].L,find_elem)==0) printf("%d元素在线性表中不存在!\n",find_elem);
            }
			getchar();getchar();
			break;
	    case 8:
            printf("请输入你希望获取前驱元素的线性表的名称：\n");
            scanf("%s",s);
            if (LocateList(Lists,s)==0) printf("未找到该线性表！\n");
			else {
                int f_p_elem,p_elem;
			    printf("输入你想要查找前驱元素的元素:\n");
			    scanf("%d",&f_p_elem);
			    switch (PriorElem(Lists.elem[LocateList(Lists,s)-1].L,f_p_elem,p_elem)) {
				    case INFEASIBLE: printf("线性表不存在！\n"); break;
				    case ERROR: printf("该元素没有前驱元素！\n"); break;
				    case OK: printf("%d元素的前驱元素是%d！\n",f_p_elem,p_elem); break;
			    }
            }
			getchar();getchar();
			break;
	    case 9:
            printf("请输入你希望获取后驱元素的线性表的名称：\n");
            scanf("%s",s);
            if (LocateList(Lists,s)==0) printf("未找到该线性表！\n");
			else {
                int f_n_elem,n_elem;
			    printf("输入你想要查找后驱元素的元素:\n");
			    scanf("%d",&f_n_elem);
			    switch (NextElem(Lists.elem[LocateList(Lists,s)-1].L,f_n_elem,n_elem)) {
				    case INFEASIBLE: printf("线性表不存在！\n"); break;
				    case ERROR: printf("该元素没有后驱元素！\n"); break;
				    case OK: printf("%d元素的后驱元素是%d！\n",f_n_elem,n_elem); break;
			    }
            }
			getchar();getchar();
			break;
	    case 10:
            printf("请输入你希望插入元素的线性表的名称：\n");
            scanf("%s",s);
            if (LocateList(Lists,s)==0) printf("未找到该线性表！\n");
			else {
                int c_elem,c_seq;
			    printf("请输入你要插入的元素：");
			    scanf("%d",&c_elem);
			    printf("你希望把它插到哪个位置：");
			    scanf("%d",&c_seq);
			    switch (ListInsert(Lists.elem[LocateList(Lists,s)-1].L,c_seq,c_elem)) {
				    case INFEASIBLE: printf("线性表不存在！\n"); break;
				    case ERROR: printf("插入位置错误！\n"); break;
				    case OK: printf("插入成功！\n"); break;
			    }
            }
			getchar();getchar();
		 	break;
	    case 11:
            printf("请输入你希望删除元素的线性表的名称：\n");
            scanf("%s",s);
            if (LocateList(Lists,s)==0) printf("未找到该线性表！\n");
		    else {
                int d_seq,d_elem;
			    printf("请输入你希望删除的元素的序号：");
			    scanf("%d",&d_seq);
			    switch (ListDelete(Lists.elem[LocateList(Lists,s)-1].L,d_seq,d_elem)) {
				    case INFEASIBLE: printf("线性表不存在!\n"); break;
				    case ERROR: printf("删除位置不正确！\n"); break;
				    case OK: printf("删除成功！\n"); break;
			    }
            }
			getchar();getchar();
		 	break;
	    case 12:
            printf("请输入你希望遍历的线性表的名称：\n");
            scanf("%s",s);
            if (LocateList(Lists,s)==0) printf("未找到该线性表！\n");
			else {
                if (!ListTraverse(Lists.elem[LocateList(Lists,s)-1].L)) printf("线性表为空！\n");
            }
			getchar();getchar();
			break;
        case 13:
            printf("请输入你希望求最大连续子数组的线性表的名称：\n");
            scanf("%s",s);
            if (LocateList(Lists,s)==0) printf("未找到该线性表！\n");
            switch (MaxSubArray(Lists.elem[LocateList(Lists,s)-1].L)) {
                case INFEASIBLE:
                    printf("线性表不存在！\n"); break;
                default:
                    printf("该线性表最大连续子数组的和为：\n%d\n",MaxSubArray(Lists.elem[LocateList(Lists,s)-1].L));
                    break;
            }
            getchar(); getchar();
            break;
        case 14:
            printf("请输入你希望求连续子数组和为k的线性表的名称：\n");
            scanf("%s",s);
            if (LocateList(Lists,s)==0) printf("未找到该线性表！\n");
            else {
                int k;
                printf("请输入数k:\n");
                scanf("%d",&k);
                switch (SubArrayNum(Lists.elem[LocateList(Lists,s)-1].L,k)) {
                    case INFEASIBLE:
                        printf("线性表bcz！\n"); break;
                    default:
                        printf("该线性表连续子数组和为%d的子数组的数目为：\n%d\n",k,SubArrayNum(Lists.elem[LocateList(Lists,s)-1].L,k));
                        break;
                }
            }
            getchar(); getchar();
            break;
        case 15:
            printf("请输入你希望从小到大排序的线性表的名称：\n");
            scanf("%s",s);
            if (LocateList(Lists,s)==0) printf("未找到该线性表！\n");
            else {
                switch(SortList(Lists.elem[LocateList(Lists,s)-1].L)) {
                    case INFEASIBLE:
                        printf("线性表不存在！\n"); break;
                    case OK:
                        printf("排序成功！\n"); break;
                }
            }
            getchar(); getchar();
            break;
        case 16:
            printf("请输入你要定位的线性表的名称：\n");
            scanf("%s",s);
            if (!LocateList(Lists,s)) {
                printf("未找到该线性表！\n");
            } else {
                printf("该线性表的序号为%d!\n",LocateList(Lists,s));
            }
            getchar(); getchar();
            break;
            case 17:
                printf("请选择操作：\n");
                printf("1. 保存线性表到文件\n");
                printf("2. 从文件加载线性表\n");
                int choice;
                scanf("%d", &choice);
                if (choice == 1) {
                    printf("输入你想保存的线性表的名称：\n");
                    char listName[30];
                    scanf("%s", listName);
                    int listIndex = LocateList(Lists, listName);
                    if (listIndex == 0) {
                        printf("未找到该线性表！\n");
                    } else {
                        printf("输入保存的文件名称：\n");
                        char s[30];
                        scanf("%s", s);
                        if (SaveList(Lists.elem[listIndex - 1].L, s) == OK) {
                            printf("线性表已成功保存到文件 %s!\n", s);
                        } else {
                            printf("保存失败！\n");
                        }
                    }
                } 
                else if (choice == 2) {
                printf("输入加载的文件名称：\n");
                char s[30];
                scanf("%s", s);
                SqList newL;
                if (LoadList(newL, s) == OK) {
                    printf("从文件 %s 加载线性表成功！\n", s);
                } else {
                printf("加载失败！\n");
                }
                } else {
                    printf("无效的选择！\n");
                }           
                getchar(); getchar();
                break;
        case 18:
            if (Lists.length<1) printf("无线性表！\n");
            else printf("以下为所有线性表：\n");
            for(int i=0; i<Lists.length; i++) {
                printf("%s:",Lists.elem[i].name);
                ListTraverse(Lists.elem[i].L);
                printf("\n");
            }
            getchar(); getchar();
            break;
		case 0:
            Lists.length=0;
         	break;
	}
  }
printf("欢迎下次再使用本系统！\n"); 
return 0;
}

//以下是众多函数

//函数一
status InitList(SqList& L)
// 线性表L不存在，构造一个空的线性表，返回OK，否则返回INFEASIBLE。
{
    if (L.elem!=NULL) return INFEASIBLE;
    else {
        L.elem=(ElemType *) malloc(sizeof(ElemType)*LIST_INIT_SIZE);
        L.length=0; L.listsize=LIST_INIT_SIZE;
        return OK;
    }
}

//函数二
status DestroyList(SqList& L) {
    if (L.elem == NULL) {
        return INFEASIBLE;
    } else {
        free(L.elem);
        L.elem = NULL;
        L.length = 0;
        L.listsize = 0;
        return OK;
    }
}

//函数三
status ClearList(SqList& L)
// 如果线性表L存在，删除线性表L中的所有元素，返回OK，否则返回INFEASIBLE。
{
    if (L.elem==NULL) return INFEASIBLE;
    else {
        L.elem=NULL;
        L.elem=(ElemType *)malloc(sizeof(ElemType));
        L.length=0;
        return OK;
    }
}

//函数四
status ListEmpty(SqList L)
// 如果线性表L存在，判断线性表L是否为空，空就返回TRUE，否则返回FALSE；如果线性表L不存在，返回INFEASIBLE。
{
    if (L.elem==NULL) return INFEASIBLE;
    else {
        if (L.length==0) return OK;
        else return ERROR;
    }
}

//函数五
status ListLength(SqList L)
// 如果线性表L存在，返回线性表L的长度，否则返回INFEASIBLE。
{
    if (L.elem==NULL) return INFEASIBLE;
    else return L.length;
}

//函数六
status GetElem(SqList L,int i,ElemType &e)
// 如果线性表L存在，获取线性表L的第i个元素，保存在e中，返回OK；如果i不合法，返回ERROR；如果线性表L不存在，返回INFEASIBLE。
{
    if (L.elem==NULL) return INFEASIBLE;
    else {
        if ((i<1)||(i>L.length)) return ERROR;
        else {
            e=L.elem[i-1];
            return OK;
        }
    }
}

//函数七
int LocateElem(SqList L,ElemType e)
// 如果线性表L存在，查找元素e在线性表L中的位置序号并返回该序号；如果e不存在，返回0；当线性表L不存在时，返回INFEASIBLE（即-1）。
{
    if (L.elem==NULL) return INFEASIBLE;
    else {
        for (int i=0;i<L.length;i++) {
            if (L.elem[i]==e) return i+1;
        }
        return 0;
    }
}

//函数八
status PriorElem(SqList L,ElemType e,ElemType &pre)
// 如果线性表L存在，获取线性表L中元素e的前驱，保存在pre中，返回OK；如果没有前驱，返回ERROR；如果线性表L不存在，返回INFEASIBLE。
{
    if (L.elem==NULL) return INFEASIBLE;
    else {
        for (int i=0;i<L.length-1;i++) {
            if (L.elem[i+1]==e) {
                pre=L.elem[i];
                return OK;
            }
        }
        return ERROR;
    }
}

//函数九
status NextElem(SqList L,ElemType e,ElemType &next)
// 如果线性表L存在，获取线性表L元素e的后继，保存在next中，返回OK；如果没有后继，返回ERROR；如果线性表L不存在，返回INFEASIBLE。
{
    if (L.elem==NULL) return INFEASIBLE;
    else {
        for (int i=0;i<L.length-1;i++) {
            if (L.elem[i]==e) {
                next=L.elem[i+1];
                return OK;
            }
        }
        return ERROR;
    }
}

//函数十
status ListInsert(SqList &L,int i,ElemType e)
// 如果线性表L存在，将元素e插入到线性表L的第i个元素之前，返回OK；当插入位置不正确时，返回ERROR；如果线性表L不存在，返回INFEASIBLE。
{
    if (L.elem==NULL) return INFEASIBLE;
    else {
        if ((i<1)||(i>L.length+1)) return ERROR;
        else {
            L.length++;
            ElemType *newbase = (ElemType *)realloc(L.elem, (L.listsize +1) * sizeof(ElemType));      
            L.elem = newbase;
            L.listsize++;
            for (int j=L.length-1;j>=i-1;j--) {
                L.elem[j+1]=L.elem[j];
            }
            L.elem[i-1]=e;
            return OK;
        }
    }
}

//函数十一
status ListDelete(SqList &L,int i,ElemType &e)
// 如果线性表L存在，删除线性表L的第i个元素，并保存在e中，返回OK；当删除位置不正确时，返回ERROR；如果线性表L不存在，返回INFEASIBLE。
{
    if (L.elem==NULL) return INFEASIBLE;
    else {
        if ((i<1)||(i>L.length-1)) return ERROR;
        else {
            e=L.elem[i-1];
            for (int j=i;j<L.length;j++) {
                L.elem[j-1]=L.elem[j];
            }
        }
        L.length--;
        return OK;
    }
}

//函数十二
status ListTraverse(SqList L)
// 如果线性表L存在，依次显示线性表中的元素，每个元素间空一格，返回OK；如果线性表L不存在，返回INFEASIBLE。
{
    if (L.elem==NULL) return 0;
    else {
        for(int i=0;i<L.length;i++) {
            printf("%d",L.elem[i]);
            if(i<L.length-1) printf(" ");
        }
        return OK;
    }
}

//函数十二
status AddList(LISTS &Lists,char ListName[])
// 只需要在Lists中增加一个名称为ListName的空线性表，线性表数据又后台测试程序插入。
{

    if(Lists.length>=10) return ERROR;
    for(int i=0;i<30;i++)
    {
    Lists.elem[Lists.length].name[i]=ListName[i];
    }
    Lists.elem[Lists.length].L.elem=NULL;
    InitList(Lists.elem[Lists.length].L);
    Lists.length++;
    return OK;
}


//函数十三
status RemoveList(LISTS &Lists,char ListName[])
// Lists中删除一个名称为ListName的线性表
{
    for (int i=0; i<Lists.length; i++) {
        if (strcmp(ListName,Lists.elem[i].name)==0) {
            if(DestroyList(Lists.elem[i].L)==INFEASIBLE) return INFEASIBLE;
            for (int j=i+1;j<Lists.length;j++) {
                Lists.elem[j-1]=Lists.elem[j];
            }
            Lists.length--;
            return OK;
        }
    }
    return ERROR;
}

//函数十四
int LocateList(LISTS Lists,char ListName[])
// 在Lists中查找一个名称为ListName的线性表，成功返回逻辑序号，否则返回0
{
    for (int i=0; i<Lists.length; i++) {
        if (strcmp(ListName,Lists.elem[i].name)==0) {
            return i+1;
        }
    }
    return 0;
}

//函数十五
status SortList(SqList& L) {
    if (L.elem == NULL)
        return INFEASIBLE;
    else {
        for (int i = 0; i < L.length - 1; i++) {
            for (int j = 0; j < L.length - 1 - i; j++) {
                int term;
                if (L.elem[j] > L.elem[j + 1]) {
                    term = L.elem[j];
                    L.elem[j] = L.elem[j + 1];
                    L.elem[j + 1] = term;
                }
            }
        }
        return OK;
    }
}

//函数十六
status MaxSubArray(SqList& L) {
    if (L.elem==NULL) return INFEASIBLE;
    else {
        int current=L.elem[0],global=L.elem[0];
        for (int i=1; i<L.length; i++) {
            current=((current+L.elem[i]>L.elem[i])?(current+L.elem[i]):(L.elem[i]));
            if (current >= global) {
                global=current;
            }
        }
        return global;
    }
}

//函数十七
status SubArrayNum(SqList& L,int k) {
    if (L.elem == NULL || L.length == 0) return INFEASIBLE;
    int count = 0;
    for (int i = 0; i < L.length; i++) {
        int current_sum = 0;
        for (int j = i; j < L.length; ++j) {
            current_sum += L.elem[j];
            if (current_sum == k) {
                count++;
            }
        }
    }
    return count;
}

//文件读写
//函数十八
status SaveList(SqList L, char FileName[]) {
    if (L.elem == NULL) return INFEASIBLE;
    FILE *fp = fopen(FileName, "w");
    if (fp == NULL) return ERROR;

    fprintf(fp, "%d ", L.length);
    for (int i = 0; i < L.length; i++) {
        fprintf(fp, "%d ", L.elem[i]);
    }

    fclose(fp);
    return OK;
}

//函数十九
status LoadList(SqList &L, char FileName[]) {
    if (L.elem != NULL) return INFEASIBLE; 
    FILE *fp = fopen(FileName, "r");
    if (fp == NULL) return ERROR;

    int length;
    if (fscanf(fp, "%d", &length) != 1) {
        fclose(fp);
        return ERROR;
    }

    L.elem = (ElemType *)malloc(length * sizeof(ElemType));
    if (L.elem == NULL) {
        fclose(fp);
        return ERROR;
    }

    for (int i = 0; i < length; i++) {
        if (fscanf(fp, "%d", &L.elem[i]) != 1) {
            free(L.elem);
            L.elem = NULL;
            fclose(fp);
            return ERROR;
        }
    }

    L.length = length;
    L.listsize = length;

    fclose(fp);
    return OK;
}