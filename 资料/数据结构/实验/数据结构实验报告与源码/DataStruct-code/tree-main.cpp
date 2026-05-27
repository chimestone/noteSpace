#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include<windows.h>
#include<cstring>

#define TRUE 1
	#define FALSE 0
	#define OK 1
	#define ERROR 0
	#define INFEASIBLE -1
	#define OVERFLOW -2

	typedef int status;
	typedef int KeyType; 
	typedef struct {
         KeyType  key;
         char others[20];
	} TElemType; //二叉树结点类型定义


	typedef struct BiTNode{  //二叉链表结点的定义
	      TElemType  data;
	      struct BiTNode *lchild,*rchild;
	} BiTNode, *BiTree;

    typedef struct {
        char name[50]; // 树的名称
        BiTree root;   // 树的根节点
    } Tree;
    
    typedef struct {
        Tree trees[100]; // 最多支持 100 棵树
        int length;      // 当前树的数量
    } TreeManager;
    
    int currentTreeIndex = -1; // 当前选择的树索引
    

    int count=0;
    int flag=-1;
    int good=0;
    
    void visit(BiTree T);
    int  ifsamekey(TElemType a[],int t);
    int calculateLength(TElemType definition[]);
    status CreateBiTree(BiTree &T,TElemType definition[]);
    status DestroyBiTree(BiTree &T);
    status ClearBiTree(BiTree &T);
    status BiTreeEmpty(BiTree &T);
    int BiTreeDepth(BiTree T);
    BiTNode* LocateNode(BiTree T,KeyType e);
    status Assign(BiTree &T,KeyType e,TElemType value);
    BiTNode* GetSibling(BiTree T,KeyType e);
    status InsertNode(BiTree &T,KeyType e,int LR,TElemType c);
    BiTNode* LocateNodeparent(BiTree T,KeyType e);
    BiTree findrs(BiTree T);
    void Delete(BiTree &T,BiTree parent,BiTree p);
    void Replace(BiTree &T,BiTree parent,BiTree p);
    void  changeplace(BiTree &T,BiTree parent,BiTree p);
    status DeleteNode(BiTree &T,KeyType e);
    status PreOrderTraverse(BiTree T,void (*visit)(BiTree));
    status InOrderTraverse(BiTree T,void (*visit)(BiTree));
    status PostOrderTraverse(BiTree T,void (*visit)(BiTree));
    status LevelOrderTraverse(BiTree T,void (*visit)(BiTree));
    int MaxPathSum(BiTree T);
    BiTNode* LowestCommonAncestor(BiTree T, KeyType e1, KeyType e2);
    status InvertTree(BiTree &T);
    void SaveBiTreeHelper(BiTree T, FILE *file);
    status SaveBiTree(BiTree T, char FileName[]);
    void LoadBiTreeHelper(BiTree &T, FILE *file);
    status LoadBiTree(BiTree &T, char FileName[]);
   
    status AddTree(TreeManager &manager, const char *name) {
        if (manager.length >= 100) {
            return ERROR; // 超过最大树数量
        }
        strcpy(manager.trees[manager.length].name, name);
        manager.trees[manager.length].root = NULL; // 初始化为空树
        manager.length++;
        return OK;
    }
    
    // 管理多棵树
    void ManageTrees(TreeManager &manager) {
        while (currentTreeIndex == -1) {
            printf("当前没有选择树，请选择操作：\n");
            printf("1. 添加树\n");
            printf("2. 选择已有树\n");
            printf("0. 退出\n");
            printf("请输入操作：");
            int initOp;
            scanf("%d", &initOp);
            if (initOp == 1) {
                printf("请输入要添加的树名称：");
                char treeName[50];
                scanf("%s", treeName);
                if (AddTree(manager, treeName) == OK) {
                    printf("树 '%s' 添加成功！\n", treeName);
                } else {
                    printf("树添加失败！\n");
                }
            } else if (initOp == 2) {
                if (manager.length == 0) {
                    printf("当前没有可选择的树，请先添加一个！\n");
                } else {
                    printf("当前已有树：\n");
                    for (int j = 0; j < manager.length; j++) {
                        printf("%d. %s\n", j + 1, manager.trees[j].name);
                    }
                    printf("请输入要选择的树序号：");
                    int index;
                    scanf("%d", &index);
                    if (index < 1 || index > manager.length) {
                        printf("无效的树序号！\n");
                    } else {
                        currentTreeIndex = index - 1;
                        printf("已选择树 '%s'\n", manager.trees[currentTreeIndex].name);
                    }
                }
            } else if (initOp == 0) {
                printf("程序退出！\n");
                exit(0);
            } else {
                printf("无效操作，请重新输入！\n");
            }
        }
    }
    
int main(){
    SetConsoleOutputCP(CP_UTF8);
    TreeManager manager = {0}; 
    ManageTrees(manager);

    
    printf("当前选择的树是：%s\n", manager.trees[currentTreeIndex].name);

    BiTree &T = manager.trees[currentTreeIndex].root;
    TElemType definition[100];
    int op=100;
    
    while(op){
        system("cls");
        printf("\n\n");
        printf("      二叉树数据结构菜单 \n");
        printf("-------------------------------------------------\n");
        printf("    	  1. 创建二叉树       7. 结点の赋值\n");
        printf("    	  2. 销毁二叉树       8.兄弟结点\n");
        printf("    	  3. 清空二叉树       9.插入结点\n");
        printf("    	  4. 验空二叉树       10.删除结点\n");
        printf("    	  5. 二叉树深度       11.前序遍历\n");
        printf("    	  6. 结点の定位       12.中序遍历\n");
        printf("    	  13.后序遍历         14.按层遍历\n");
        printf("          15.最大路径和       16.最近公共祖先\n");
        printf("          17.翻转二叉树       18.保存二叉树\n");
        printf("          19.加载二叉树       20.增加二叉树\n");
        printf("          21.删除二叉树       22.返回总菜单\n");
        printf("-------------------------------------------------\n");
        printf("请选择你的操作:[1~21]:");
        scanf("%d",&op);
        switch(op){

            case 1: {
                 count=0;
                 flag=-1;
                good=0;
                    printf("请输入数据，以关键字为-1结束\n");
                    for (int i = 0; i < 100; i++) {
                        definition[i].key = 0;
                    }
                    TElemType definition[100];
                    
                    int ans,i=0;
                    do {
	                scanf("%d%s",&definition[i].key,definition[i].others);
                    } while (definition[i++].key!=-1);
                   
                    ans=CreateBiTree(T,definition);
                    if (ans==OK)
                    {
                       printf("初始化完成\n");
                    }
                    else printf("关键字不唯一\n");

                    getchar();  getchar();
                    break;
                
              
              
            }
            case 2: {
                if (DestroyBiTree(T) == OK) {
                    printf("二叉树销毁成功！\n");
                } else {
                    printf("二叉树销毁失败！\n");
                }
                getchar(); getchar();
                break;
            }
            case 3: {
                if (ClearBiTree(T) == OK) {
                    printf("二叉树清空成功！\n");
                } else {
                    printf("二叉树清空失败！\n");
                }
                getchar(); getchar();
                break;
            }
            case 4: {
                if (BiTreeEmpty(T) == OK) {
                    printf("二叉树为空！\n");
                } else {
                    printf("二叉树不为空！\n");
                }
                getchar(); getchar();
                break;
            }
            case 5: {
                int depth = BiTreeDepth(T);
                printf("二叉树的深度为：%d\n", depth);
                getchar(); getchar();
                break;
            }
            case 6: {
                KeyType e;
                printf("请输入要查找的关键字：\n");
                scanf("%d", &e);
                BiTNode* node = LocateNode(T, e);
                if (node) {
                    printf("找到结点：%d, %s\n", node->data.key, node->data.others);
                } else {
                    printf("未找到关键字为 %d 的结点！\n", e);
                }
                getchar(); getchar();
                break;
            }
            case 7: {
                KeyType e;
                TElemType value;
                printf("请输入要赋值的关键字：\n");
                scanf("%d", &e);
                printf("请输入新值（key 和 others）：\n");
                scanf("%d %s", &value.key, value.others);
                if (Assign(T, e, value) == OK) {
                    printf("赋值成功！\n");
                } else {
                    printf("赋值失败！\n");
                }
                getchar(); getchar();
                break;
            }
            case 8: {
                KeyType e;
                printf("请输入要查找兄弟结点的关键字：\n");
                scanf("%d", &e);
                BiTNode* sibling = GetSibling(T, e);
                if (sibling) {
                    printf("兄弟结点为：%d, %s\n", sibling->data.key, sibling->data.others);
                } else {
                    printf("未找到兄弟结点！\n");
                }
                getchar(); getchar();
                break;
            }
            case 9: {
                KeyType e;
                int LR;
                TElemType c;
                printf("请输入父结点关键字：\n");
                scanf("%d", &e);
                printf("请输入插入位置（0 表示左，1 表示右）：\n");
                scanf("%d", &LR);
                printf("请输入插入结点的值（key 和 others）：\n");
                scanf("%d %s", &c.key, c.others);
                if (InsertNode(T, e, LR, c) == OK) {
                    printf("插入结点成功！\n");
                } else {
                    printf("插入结点失败！\n");
                }
                getchar(); getchar();
                break;
            }
            case 10: {
                KeyType e;
                printf("请输入要删除的结点关键字：\n");
                scanf("%d", &e);
                if (DeleteNode(T, e) == OK) {
                    printf("删除结点成功！\n");
                } else {
                    printf("删除结点失败！\n");
                }
                getchar(); getchar();
                break;
            }
            case 11: {
                printf("先序遍历结果：\n");
                PreOrderTraverse(T, visit);
                printf("\n");
                getchar(); getchar();
                break;
            }
            case 12: {
                printf("中序遍历结果：\n");
                InOrderTraverse(T, visit);
                printf("\n");
                getchar(); getchar();
                break;
            }
            case 13: {
                printf("后序遍历结果：\n");
                PostOrderTraverse(T, visit);
                printf("\n");
                getchar(); getchar();
                break;
            }
            case 14: {
                printf("层序遍历结果：\n");
                LevelOrderTraverse(T, visit);
                printf("\n");
                getchar(); getchar();
                break;
            }
           
            case 15: {
                int maxSum = MaxPathSum(T);
                printf("二叉树的最大路径和为：%d\n", maxSum);
                getchar(); getchar();
                break;
            }
            case 16: {
                KeyType e1, e2;
                printf("请输入两个节点的关键字：\n");
                scanf("%d %d", &e1, &e2);
                BiTNode* ancestor = LowestCommonAncestor(T, e1, e2);
                if (ancestor) {
                    printf("最近公共祖先的关键字为：%d\n", ancestor->data.key);
                } else {
                    printf("未找到最近公共祖先！\n");
                }
                getchar(); getchar();
                break;
            }
            case 17: {
                if (InvertTree(T) == OK) {
                    printf("二叉树翻转成功！\n");
                } else {
                    printf("二叉树翻转失败！\n");
                }
                getchar(); getchar();
                break;
            }
            case 18: {
                char fileName[100];
                printf("请输入保存文件的名称：\n");
                scanf("%s", fileName);
                if (SaveBiTree(T, fileName) == OK) {
                    printf("二叉树保存成功！\n");
                } else {
                    printf("二叉树保存失败！\n");
                }
                getchar(); getchar();
                break;
            }
            case 19: {
                char fileName[100];
                printf("请输入加载文件的名称：\n");
                scanf("%s", fileName);
                if (LoadBiTree(T, fileName) == OK) {
                    printf("二叉树加载成功！\n");
                } else {
                    printf("二叉树加载失败！\n");
                }
                getchar(); getchar();
                break;
            }
            
            case 20: {
                printf("请输入要添加的树名称：\n");
                char treeName[50];
                scanf("%s", treeName);
                if (AddTree(manager, treeName) == OK) {
                    printf("树 '%s' 添加成功！\n", treeName);
                } else {
                    printf("树添加失败！可能已达到最大树数量限制。\n");
                }
                getchar(); getchar();
                break;
            }

            case 21: {
                if (manager.length == 0) {
                    printf("当前没有可删除的树！\n");
                } else {
                    printf("当前已有树：\n");
                    for (int j = 0; j < manager.length; j++) {
                        printf("%d. %s\n", j + 1, manager.trees[j].name);
                    }
                    printf("请输入要删除的树序号：\n");
                    int index;
                    scanf("%d", &index);
                    if (index < 1 || index > manager.length) {
                        printf("无效的树序号！\n");
                    } else {
                        // 删除树
                        for (int j = index - 1; j < manager.length - 1; j++) {
                            manager.trees[j] = manager.trees[j + 1];
                        }
                        manager.length--;
                        currentTreeIndex = -1; // 重置当前选择的树
                        printf("树已删除！\n");
                    }
                }
                getchar(); getchar();
                break;
            }
            case 22: {
                currentTreeIndex = -1; // 重置当前选择的树索引
                ManageTrees(manager); // 调用多树管理函数
                T = manager.trees[currentTreeIndex].root; // 更新当前树的根节点
                printf("已返回多树选择界面，当前选择的树是：%s\n", manager.trees[currentTreeIndex].name);
                getchar(); getchar();
                break;
            }

            default: {
                printf("无效的操作，请重新输入！\n");
                getchar(); getchar();
                break;
            }
        }
        
    }

}











void visit(BiTree T)
{
    printf(" %d,%s",T->data.key,T->data.others);
}

int  ifsamekey(TElemType a[],int t){
    for(int i=0;i<t;i++){
          for(int j=i+1;j<t;j++){
                if(a[i].key==a[j].key&&a[i].key!=0)  return 1;
          }
    }
    return 0;
} 
int calculateLength(TElemType definition[]) {
    int length = 0;
    while (definition[length].key != -1) {  // 遇到 key == -1 时结束
        length++;
    }
    return length;
}

status CreateBiTree(BiTree &T,TElemType definition[])
/*根据带空枝的二叉树先根遍历序列definition构造一棵二叉树，将根节点指针赋值给T并返回OK，
如果有相同的关键字，返回ERROR。此题允许通过增加其它函数辅助实现本关任务*/
{
  // 请在这里补充代码，完成本关任务
  /********** Begin ********/
  TElemType *p=definition;
  int t=calculateLength(definition);
  if(ifsamekey(definition,t)) return ERROR;
  if(definition[count].key==0){
    T=NULL;
    count++;
    return OK;
  }
 if(!(T=(BiTree)malloc(sizeof(BiTNode))))  return ERROR;
 else{
    T->data=definition[count++];
    CreateBiTree(T->lchild,definition);
    CreateBiTree(T->rchild,definition);
    return OK;
 }

  /********** End **********/
}
status ClearBiTree(BiTree &T)
{
    if (T == NULL) 
        return OK;  // 空树直接返回
    
    DestroyBiTree(T->lchild);  
    DestroyBiTree(T->rchild);  
    T=NULL;

    
    return OK;
}
status DestroyBiTree(BiTree &T) {
    if (T == NULL) {
        return OK; 
    }
    DestroyBiTree(T->lchild); // 递归销毁左子树
    DestroyBiTree(T->rchild); // 递归销毁右子树
    free(T); // 释放当前节点
    T = NULL; // 将当前节点指针置为空
    return OK;
}
status BiTreeEmpty(BiTree &T){
    if(T==NULL)  return OK;
    
    else return ERROR;
}
int BiTreeDepth(BiTree T)
{
    if (T == NULL) {
        return 0;  
    }
    int leftDepth = BiTreeDepth(T->lchild);
    int rightDepth = BiTreeDepth(T->rchild);

    return (leftDepth > rightDepth ? leftDepth : rightDepth) + 1;
}
BiTNode* LocateNode(BiTree T,KeyType e)
//查找结点
{
    // 请在这里补充代码，完成本关任务
    /********** Begin *********/
    if(T==NULL)  return NULL;
    if(T->data.key==e)  return T;
    BiTree p;
    p=LocateNode(T->lchild,e);
    if(p!=NULL)  return p;
    return LocateNode(T->rchild,e);
    

    /********** End **********/
}
status Assign(BiTree &T,KeyType e,TElemType value)
//实现结点赋值。此题允许通过增加其它函数辅助实现本关任务
{
    // 请在这里补充代码，完成本关任务
    /********** Begin *********/
    BiTree p=LocateNode(T,e);
    BiTree key=NULL;
    if(e!=value.key){
        key=LocateNode(T,value.key);
    }
    if(p==NULL||key!=NULL)  return ERROR;
    p->data=value;
    return OK;


    /********** End **********/
}
BiTNode* GetSibling(BiTree T,KeyType e)
//实现获得兄弟结点
{
    if (T == NULL) return NULL;
    if (T->data.key == e) return NULL; 
    BiTNode* parent = LocateNodeparent(T, e);
    if (parent == NULL) return NULL;
    if (parent->lchild != NULL && parent->lchild->data.key == e) {
        return parent->rchild;
    } else if (parent->rchild != NULL && parent->rchild->data.key == e) {
        return parent->lchild;
    }
    return NULL;
}
status InsertNode(BiTree &T,KeyType e,int LR,TElemType c)
//插入结点。此题允许通过增加其它函数辅助实现本关任务
{
    // 请在这里补充代码，完成本关任务
    /********** Begin *********/
    BiTree p=(BiTree)malloc(sizeof(BiTNode));
    p->data=c;
    p->lchild=NULL;
    p->rchild=NULL;
    if(T==NULL)  return ERROR;
    if(LR==-1) {
        p->rchild=T;
        T=p;
        return OK;
    }else{
        BiTree location=NULL;
        location=LocateNode(T,c.key);
        if(location!=NULL)  return ERROR;
        location=LocateNode(T,e);
        if(location==NULL)  return ERROR;
        if(LR==0){
            p->rchild=location->lchild;
            location->lchild=p;
        }else if(LR==1){
            p->rchild=location->rchild;
            location->rchild=p;
        }
        return OK;
    }

    /********** End **********/
}
BiTNode* LocateNodeparent(BiTree T,KeyType e)
 //查找结点
 {
     if (T == NULL) return NULL;
     if (T->lchild != NULL && T->lchild->data.key == e) {
        flag=0;
        return T;
    }
     if (T->rchild != NULL && T->rchild->data.key == e) {
        flag=1;
        return T;}
     BiTNode* leftResult = LocateNodeparent(T->lchild, e);
     if (leftResult != NULL) return leftResult;
     return LocateNodeparent(T->rchild, e);
 }
BiTree findrs(BiTree T){
    if(T->rchild==NULL) return T;
    return findrs(T->rchild);
}
  void Delete(BiTree &T,BiTree parent,BiTree p){
      if(flag==0){
        parent->lchild=NULL;
        free(p);
        good=1;
      }else if(flag==1){
        parent->rchild=NULL;
        free(p);
        good=1;
      }
  }
  void Replace(BiTree &T,BiTree parent,BiTree p){
    BiTree newpoint=(p->lchild==NULL?p->rchild:p->lchild);
    if(flag==0){
        parent->lchild=newpoint;
        good=1;
       }else if(flag==1){
        parent->rchild=newpoint;
        good=1;
       }
       free(p);
  }
  void  changeplace(BiTree &T,BiTree parent,BiTree p){
      BiTree rs=findrs(p->lchild);
    if (parent == NULL) { 
        rs->rchild = p->rchild;  
        T = p->lchild;          
        free(p);                
        good = 1;
        return;
    }
      if(flag==0){
        parent->lchild=p->lchild;
        rs->rchild=p->rchild;
        good=1;
      }else if(flag==1){
        parent->rchild=p->lchild;
        rs->rchild=p->rchild;
        good=1;
      }
      free(p);
  }
 status DeleteNode(BiTree &T,KeyType e)
//删除结点。此题允许通过增加其它函数辅助实现本关任务
{
    // 请在这里补充代码，完成本关任务
    /********** Begin *********/
    if(T==NULL)  return ERROR;
    BiTree p=LocateNode(T,e);
    if(p==NULL)  return ERROR;
    int degree=0;
    if(p->lchild!=NULL) degree++;
    if(p->rchild!=NULL)  degree++;
    BiTree parent=LocateNodeparent(T,e);
    switch(degree){
        case 0:
        Delete(T,parent,p);
        break;
        case 1:
        Replace(T,parent,p);
        break;
        case 2:
        changeplace(T,parent,p);
        break;
        default:
        return ERROR;
    }
    if(good)  return OK;
    else return ERROR;
    /********** End **********/
}
status PreOrderTraverse(BiTree T,void (*visit)(BiTree))
//先序遍历二叉树T
{
    // 请在这里补充代码，完成本关任务
    /********** Begin *********/
    if(T){
         visit(T);
     PreOrderTraverse(T->lchild,visit);
     PreOrderTraverse(T->rchild,visit);
    }
    return OK;
    /********** End **********/
}
status InOrderTraverse(BiTree T,void (*visit)(BiTree))
//中序遍历二叉树T
{
    // 请在这里补充代码，完成本关任务
    /********** Begin *********/
    if(T){
        InOrderTraverse(T->lchild,visit);
        visit(T);
        InOrderTraverse(T->rchild,visit);
    }
    return OK;

    /********** End **********/
}
status PostOrderTraverse(BiTree T,void (*visit)(BiTree))
//后序遍历二叉树T
{
    // 请在这里补充代码，完成本关任务
    /********** Begin *********/
    if(T){
        PostOrderTraverse(T->lchild,visit);
        PostOrderTraverse(T->rchild,visit);
        visit(T);
    }
    return OK;

    /********** End **********/
}
status LevelOrderTraverse(BiTree T,void (*visit)(BiTree))
 //按层遍历二叉树T
 {
     // 请在这里补充代码，完成本关任务
     /********** Begin *********/
    BiTree Queue[10000];
    int rear=0,front=0;
    if(T!=NULL)  Queue[rear++]=T;
    while(rear!=front){
        T=Queue[front++];
        visit(T);
        if(T->lchild!=NULL) Queue[rear++]=T->lchild;
        if(T->rchild!=NULL) Queue[rear++]= T->rchild;
    }
    return OK;
     /********** End **********/
 }

 
 int MaxPathSum(BiTree T) {
    if (T == NULL) {
        return 0; // 空节点的路径和为 0
    }
    int leftSum = MaxPathSum(T->lchild); // 递归计算左子树的最大路径和
    int rightSum = MaxPathSum(T->rchild); // 递归计算右子树的最大路径和
    return T->data.key + (leftSum > rightSum ? leftSum : rightSum); // 当前节点值加上左右子树中较大的路径和
}

BiTNode* LowestCommonAncestor(BiTree T, KeyType e1, KeyType e2) {
    if (T == NULL) {
        return NULL; // 如果当前节点为空，返回 NULL
    }
    if (T->data.key == e1 || T->data.key == e2) {
        return T; // 如果当前节点是 e1 或 e2，返回当前节点
    }

    // 递归查找左子树和右子树
    BiTNode* left = LowestCommonAncestor(T->lchild, e1, e2);
    BiTNode* right = LowestCommonAncestor(T->rchild, e1, e2);

    if (left != NULL && right != NULL) {
        return T; // 如果 e1 和 e2 分别在左右子树中，当前节点是最近公共祖先
    }

    return (left != NULL) ? left : right; // 否则返回非空的子树
}

status InvertTree(BiTree &T) {
    if (T == NULL) {
        return OK; // 空树直接返回
    }

    // 交换当前节点的左右子树
    BiTree temp = T->lchild;
    T->lchild = T->rchild;
    T->rchild = temp;

    // 递归翻转左右子树
    InvertTree(T->lchild);
    InvertTree(T->rchild);

    return OK;
}

void SaveBiTreeHelper(BiTree T, FILE *file) {
    if (T == NULL) {
        fprintf(file, "0 null\n");  
        return;
    }
    fprintf(file, "%d %s\n", T->data.key, T->data.others);  
    SaveBiTreeHelper(T->lchild, file);  
    SaveBiTreeHelper(T->rchild, file);  
}


status SaveBiTree(BiTree T, char FileName[]) {
    FILE *file = fopen(FileName, "w");
    if (file == NULL) {
        return ERROR;  
    }
    SaveBiTreeHelper(T, file);
    fclose(file);
    return OK;
}


void LoadBiTreeHelper(BiTree &T, FILE *file) {
    int key;
    char others[20];
    if (fscanf(file, "%d %s", &key, others) != 2) {
        T = NULL;
        return;
    }
    if (key == 0) { 
        T = NULL;
        return;
    }
    T = (BiTree)malloc(sizeof(BiTNode));
    T->data.key = key;
    strcpy(T->data.others, others);
    LoadBiTreeHelper(T->lchild, file);  
    LoadBiTreeHelper(T->rchild, file);  
}

status LoadBiTree(BiTree &T, char FileName[]) {
    FILE *file = fopen(FileName, "r");
    if (file == NULL) {
        return ERROR;  
    }
    LoadBiTreeHelper(T, file);
    fclose(file);
    return OK;
}