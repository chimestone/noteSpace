#include<stdio.h>
#include<cstring>
#include<windows.h>
#include<malloc.h>
#include<stdlib.h>
#include <vector>
#include <queue>
	#define TRUE 1
	#define FALSE 0
	#define OK 1
	#define ERROR 0
	#define INFEASIBLE -1
	#define OVERFLOW -2
	#define MAX_VERTEX_NUM 20
	typedef int status;
	typedef int KeyType; 
   typedef enum {DG,DN,UDG,UDN} GraphKind;
	typedef struct {
         KeyType  key;
         char others[20];
	} VertexType; //顶点类型定义

	typedef struct ArcNode {         //表结点类型定义
   		 int adjvex;              //顶点位置编号 
    	 struct ArcNode  *nextarc;	   //下一个表结点指针
	} ArcNode;
	typedef struct VNode{				//头结点及其数组类型定义
   		 VertexType data;       	//顶点信息
    	 ArcNode *firstarc;      	 //指向第一条弧
    	} VNode,AdjList[MAX_VERTEX_NUM];
	typedef  struct {  //邻接表的类型定义
        AdjList vertices;     	 //头结点数组
        int vexnum,arcnum;   	  //顶点数、弧数
        GraphKind  kind;        //图的类型
       } ALGraph;

       void visit(VertexType v)
{
    printf(" %d %s",v.key,v.others);
}

       int Numof_arcnum(KeyType a[][2]);
       int Numof_vexnum(VertexType a[]);
       int isunique(VertexType a[],int vexnum);
       status CreateCraph(ALGraph &G,VertexType V[],KeyType VR[][2]);
       status DestroyGraph(ALGraph &G);
       int LocateVex(ALGraph G,KeyType u);
       int isuniqueofG(ALGraph G);
       status PutVex(ALGraph &G,KeyType u,VertexType value);
       int FirstAdjVex(ALGraph G,KeyType u);
       int NextAdjVex(ALGraph G, KeyType v, KeyType w);
       status InsertVex(ALGraph &G,VertexType v);
       status DeleteVex(ALGraph &G, KeyType v);
       int isuniquenew(ALGraph G,int i,int j);
       status InsertArc(ALGraph &G,KeyType v,KeyType w);
       status DeleteArc(ALGraph &G,KeyType v,KeyType w);
       void DFS(ALGraph G,int i,bool visited[],void (*visit)(VertexType));
       status DFSTraverse(ALGraph &G,void (*visit)(VertexType));
       status BFSTraverse(ALGraph &G,void (*visit)(VertexType));
       std::vector<int> VerticesSetLessThanK(ALGraph &G, KeyType v, int k) ;
       int ShortestPathLength(ALGraph &G, KeyType v, KeyType w);
       int ConnectedComponentsNums(ALGraph &G);
       status SaveGraph(ALGraph G, char FileName[]);
       status LoadGraph(ALGraph &G, char FileName[]);
        void visit(VertexType v);

      typedef struct {
    ALGraph graphs[MAX_VERTEX_NUM]; // 存储多个图
    char names[MAX_VERTEX_NUM][50]; // 存储每个图的名称
    int length;                     // 当前图的数量
} GraphManager;

void ManageGraphs(GraphManager &manager) {
    int currentGraphIndex = -1;

    while (currentGraphIndex == -1) {
        printf("当前没有选择图，请选择操作：\n");
        printf("1. 添加图\n");
        printf("2. 选择已有图\n");
        printf("0. 退出\n");
        printf("请输入操作：");
        int initOp;
        scanf("%d", &initOp);

        if (initOp == 1) {
            if (manager.length >= MAX_VERTEX_NUM) {
                printf("图的数量已达到上限，无法添加更多图！\n");
                continue;
            }

            printf("请输入要添加的图名称：");
            char graphName[50];
            scanf("%s", graphName);

            // 添加图但不初始化
            manager.graphs[manager.length].vexnum = 0;
            manager.graphs[manager.length].arcnum = 0;
            manager.graphs[manager.length].kind = UDG; // 默认类型
            strcpy(manager.names[manager.length], graphName); // 保存图名称
            manager.length++;
            printf("图 '%s' 添加成功！\n", graphName);
        } else if (initOp == 2) {
            if (manager.length == 0) {
                printf("当前没有可选择的图，请先添加一个！\n");
            } else {
                printf("当前已有图：\n");
                for (int j = 0; j < manager.length; j++) {
                    printf("%d. %s\n", j + 1, manager.names[j]); // 显示图的名称
                }
                printf("请输入要选择的图序号：");
                int index;
                scanf("%d", &index);
                if (index < 1 || index > manager.length) {
                    printf("无效的图序号！\n");
                } else {
                    currentGraphIndex = index - 1;
                    printf("已选择图 '%s'\n", manager.names[currentGraphIndex]);
                }
            }
        } else if (initOp == 0) {
            printf("程序退出！\n");
            exit(0);
        } else {
            printf("无效操作，请重新输入！\n");
        }
    }

    printf("当前选择的图是：%s\n", manager.names[currentGraphIndex]);
}
int SelectGraph(GraphManager &manager) {
    if (manager.length == 0) {
        printf("当前没有可选择的图，请先添加一个！\n");
        return -1; // 返回 -1 表示没有可选择的图
    }

    printf("当前已有图：\n");
    for (int i = 0; i < manager.length; i++) {
        printf("%d. 图 %d\n", i + 1, i + 1);
    }

    printf("请输入要选择的图序号：");
    int index;
    scanf("%d", &index);

    if (index < 1 || index > manager.length) {
        printf("无效的图序号！\n");
        return -1; // 返回 -1 表示选择无效
    }

    printf("已选择图 %d\n", index);
    return index - 1; // 返回图的索引
}
       int main() {
        SetConsoleOutputCP(CP_UTF8); 
         GraphManager manager = {0}; 
         ManageGraphs(manager);
        ALGraph G; 
        int op = 100;
    
        while (op) {
            system("cls");
            printf("\n\n");
	printf("      Menu for Linear Table On Sequence Structure \n");
	printf("-------------------------------------------------\n");
	printf("    	  1. CreateGraph                 2. DestroyGraph\n");
	printf("    	  3. LocateVex                   4. PutVex\n");
	printf("    	  5. FirstAdjVex                 6. NextAdjVex \n");
	printf("    	  7. InsertVex                   8. DeleteVex\n");
	printf("    	  9. InsertArc                   10. DeleteArc\n");
	printf("    	  11. DFSTraverse                12. BFSTraverse\n");
	printf("          13.VerticesSetLessThank        14.ShortestPathLength\n");
	printf("          15.ConnectedComponentsNums     16.SaveGraph\n");
	printf("          17.LoadGraph                   18.ArcTraverse\n");
	printf("    	  19.AssignVex\n");
	printf("    	  0.Exit\n");
	printf("-------------------------------------------------\n");
	printf("    请选择你的操作:");
            printf("选择操作[1~19]: ");
            scanf("%d", &op);
    
            switch (op) {
                case 1: { // 创建图
                    VertexType V[100];
                    KeyType VR[100][2];
                    printf("请输入顶点信息（key 和 others），以 key=-1 结束：\n");
                    int i = 0;
                    do {
                        scanf("%d%s", &V[i].key, V[i].others);
                    } while (V[i++].key != -1);
    
                    printf("请输入边信息（起点和终点 key），以 -1 -1 结束：\n");
                    int j = 0;
                    do {
                        scanf("%d%d", &VR[j][0], &VR[j][1]);
                    } while (VR[j++][0] != -1);
    
                    if (CreateCraph(G, V, VR) == OK) {
                        printf("图创建成功！\n");
                    } else {
                        printf("图创建失败！\n");
                    }
                    getchar(); getchar();
                    break;
                }
                case 2: { // 销毁图
                    if (DestroyGraph(G) == OK) {
                        printf("图销毁成功！\n");
                    } else {
                        printf("图销毁失败！\n");
                    }
                    getchar(); getchar();
                    break;
                }
                case 3: { // 查找顶点
                    KeyType u;
                    printf("请输入要查找的顶点关键字：\n");
                    scanf("%d", &u);
                    int pos = LocateVex(G, u);
                    if (pos != -1) {
                        printf("顶点 '%d' 的位置序号为：%d\n", u, pos);
                    } else {
                        printf("未找到关键字为 '%d' 的顶点！\n", u);
                    }
                    getchar(); getchar();
                    break;
                }
                case 4: { // 顶点赋值
                    KeyType u;
                    VertexType value;
                    printf("请输入要赋值的顶点关键字：\n");
                    scanf("%d", &u);
                    printf("请输入新值（key 和 others）：\n");
                    scanf("%d%s", &value.key, value.others);
                    if (PutVex(G, u, value) == OK) {
                        printf("赋值成功！\n");
                    } else {
                        printf("赋值失败！\n");
                    }
                    getchar(); getchar();
                    break;
                }
                case 5: { // 获得第一邻接点
                    KeyType u;
                    printf("请输入顶点关键字：\n");
                    scanf("%d", &u);
                    int firstAdj = FirstAdjVex(G, u);
                    if (firstAdj != -1) {
                        printf("顶点 '%d' 的第一个邻接点位序为：%d\n", u, firstAdj);
                    } else {
                        printf("顶点 '%d' 没有邻接点或不存在！\n", u);
                    }
                    getchar(); getchar();
                    break;
                }
                case 6: { // 获得下一邻接点
                    KeyType v, w;
                    printf("请输入顶点 v 和邻接点 w 的关键字：\n");
                    scanf("%d%d", &v, &w);
                    int nextAdj = NextAdjVex(G, v, w);
                    if (nextAdj != -1) {
                        printf("顶点 '%d' 的相对于 '%d' 的下一个邻接点位序为：%d\n", v, w, nextAdj);
                    } else {
                        printf("未找到下一个邻接点或顶点不存在！\n");
                    }
                    getchar(); getchar();
                    break;
                }
                case 7: { // 插入顶点
                    VertexType v;
                    printf("请输入要插入的顶点信息（key 和 others）：\n");
                    scanf("%d%s", &v.key, v.others);
                    if (InsertVex(G, v) == OK) {
                        printf("顶点插入成功！\n");
                    } else {
                        printf("顶点插入失败！\n");
                    }
                    getchar(); getchar();
                    break;
                }
                case 8: { // 删除顶点
                    KeyType v;
                    printf("请输入要删除的顶点关键字：\n");
                    scanf("%d", &v);
                    if (DeleteVex(G, v) == OK) {
                        printf("顶点删除成功！\n");
                    } else {
                        printf("顶点删除失败！\n");
                    }
                    getchar(); getchar();
                    break;
                }
                case 9: { // 插入弧
                    KeyType v, w;
                    printf("请输入要插入的弧（起点 key 和终点 key）：\n");
                    scanf("%d%d", &v, &w);
                    if (InsertArc(G, v, w) == OK) {
                        printf("弧插入成功！\n");
                    } else {
                        printf("弧插入失败！\n");
                    }
                    getchar(); getchar();
                    break;
                }
                case 10: { // 删除弧
                    KeyType v, w;
                    printf("请输入要删除的弧（起点 key 和终点 key）：\n");
                    scanf("%d%d", &v, &w);
                    if (DeleteArc(G, v, w) == OK) {
                        printf("弧删除成功！\n");
                    } else {
                        printf("弧删除失败！\n");
                    }
                    getchar(); getchar();
                    break;
                }
                case 11: { // 深度优先遍历
                    KeyType startKey;
                    printf("请输入深度优先遍历的起始顶点关键字：\n");
                    scanf("%d", &startKey);

                    int startIndex = LocateVex(G, startKey);
                    if (startIndex == -1) {
                        printf("起始顶点 '%d' 不存在！\n", startKey);
                    } else {
                    printf("深度优先遍历结果：\n");
                    bool visited[MAX_VERTEX_NUM] = {false};
                    DFS(G, startIndex, visited, visit);
                    printf("\n遍历完成！\n");
                    }
                    getchar(); getchar();
                    break;
                        }
               case 12: { // 广度优先遍历
                    KeyType startKey;
                    printf("请输入广度优先遍历的起始顶点关键字：\n");
                    scanf("%d", &startKey);

                    int startIndex = LocateVex(G, startKey);
                    if (startIndex == -1) {
                    printf("起始顶点 '%d' 不存在！\n", startKey);
                    } else {
                    printf("广度优先遍历结果：\n");
                    bool visited[MAX_VERTEX_NUM] = {false};
                    int QUEUE[MAX_VERTEX_NUM], head = 0, tail = 0;

                    visited[startIndex] = true;
                    visit(G.vertices[startIndex].data);
                    QUEUE[tail++] = startIndex;

                    while (head != tail) {
                        int current = QUEUE[head++];
                        ArcNode *p = G.vertices[current].firstarc;
                        while (p != NULL) {
                        if (!visited[p->adjvex]) {
                            visited[p->adjvex] = true;
                            visit(G.vertices[p->adjvex].data);
                            QUEUE[tail++] = p->adjvex;
                        }
                        p = p->nextarc;
                }
        }
        printf("\n遍历完成！\n");
    }
                 getchar(); getchar();
                break;
}
                case 13: { // 距离小于 k 的顶点集合
                    KeyType v;
                    int k;
                    printf("请输入起始顶点关键字：\n");
                    scanf("%d", &v);
                    printf("请输入距离阈值 k：\n");
                    scanf("%d", &k);
                    std::vector<int> result = VerticesSetLessThanK(G, v, k);
                    if (result.empty()) {
                        printf("没有与顶点 %d 距离小于 %d 的顶点！\n", v, k);
                    } else {
                        printf("与顶点 %d 距离小于 %d 的顶点集合为：", v, k);
                        for (int key : result) {
                            printf("%d ", key);
                        }
                        printf("\n");
                    }
                    getchar(); getchar();
                    break;
                }
                case 14: { // 顶点间最短路径长度
                    KeyType v, w;
                    printf("请输入起始顶点关键字：\n");
                    scanf("%d", &v);
                    printf("请输入目标顶点关键字：\n");
                    scanf("%d", &w);
                    int length = ShortestPathLength(G, v, w);
                    if (length != -1) {
                        printf("顶点 %d 和顶点 %d 之间的最短路径长度为：%d\n", v, w, length);
                    } else {
                        printf("顶点 %d 和顶点 %d 之间不存在路径！\n", v, w);
                    }
                    getchar(); getchar();
                    break;
                }
                case 15: { // 图的连通分量个数
                    int numComponents = ConnectedComponentsNums(G);
                    printf("图的连通分量个数为：%d\n", numComponents);
                    getchar(); getchar();
                    break;
                }
                case 16: { // 保存图到文件
                    char fileName[100];
                    printf("请输入保存文件的名称：\n");
                    scanf("%s", fileName);
                    if (SaveGraph(G, fileName) == OK) {
                        printf("图保存成功！\n");
                    } else {
                        printf("图保存失败！\n");
                    }
                    getchar(); getchar();
                    break;
                }
                case 17: { // 从文件加载图
                    char fileName[100];
                    printf("请输入加载文件的名称：\n");
                    scanf("%s", fileName);
                    if (LoadGraph(G, fileName) == OK) {
                        printf("图加载成功！\n");
                    } else {
                        printf("图加载失败！\n");
                    }
                    getchar(); getchar();
                    break;
                }
                 case 18: { 
                    if (manager.length == 0) {
                        printf("当前没有可选择的图，请先添加一个！\n");
                    } else {
                    printf("当前已有图：\n");
                    for (int i = 0; i < manager.length; i++) {
                     printf("%d. %s\n", i + 1, manager.names[i]); 
                    }

                     printf("请输入要选择的图序号：");
                    int selectedGraphIndex;
                    scanf("%d", &selectedGraphIndex);

                    if (selectedGraphIndex < 1 || selectedGraphIndex > manager.length) {
                     printf("无效的图序号！\n");
                    } else {
                    G = manager.graphs[selectedGraphIndex - 1]; 
                    printf("已选择图 '%s'\n", manager.names[selectedGraphIndex - 1]);
                    }
                 }
                getchar(); getchar();
                 break;
                }
               case 19: { // 添加新图
                    if (manager.length >= MAX_VERTEX_NUM) {
                        printf("图的数量已达到上限，无法添加更多图！\n");
                    } else {
                    printf("请输入要添加的图名称：");
                    char graphName[50];
                    scanf("%s", graphName);

        
                    manager.graphs[manager.length].vexnum = 0;
                    manager.graphs[manager.length].arcnum = 0;
                    manager.graphs[manager.length].kind = UDG;
                    strcpy(manager.names[manager.length], graphName);
                    manager.length++;

                    printf("图 '%s' 添加成功！当前图总数：%d\n", graphName, manager.length);
                    }
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
    
        return 0;
    }

int Numof_arcnum(KeyType a[][2]){
    int i=0;
    while(a[i][0]!=-1){
        i++;
    }
    return i;
}
int Numof_vexnum(VertexType a[]){
    int i=0;
    while(a[i].key!=-1){
        i++;
    }
    return i;
}
int isunique(VertexType a[],int vexnum){
    int j;
   for(int i=0;i<vexnum;i++){
       for(j=i+1;j<vexnum;j++){
        if(a[i].key==a[j].key)  return 0;
       }
   }
   return 1;
}
status CreateCraph(ALGraph &G,VertexType V[],KeyType VR[][2])
/*根据V和VR构造图T并返回OK，如果V和VR不正确，返回ERROR
如果有相同的关键字，返回ERROR。此题允许通过增加其它函数辅助实现本关任务*/
{
    // 请在这里补充代码，完成本关任务
    /********** Begin *********/
    G.arcnum=Numof_arcnum(VR);
    G.vexnum=Numof_vexnum(V);
    if(G.vexnum==0)  return ERROR;
    if(!isunique(V,G.vexnum)){
        return ERROR;
    }
    for(int i=0;i<G.vexnum;i++){
        G.vertices[i].data=V[i];
        G.vertices[i].firstarc=NULL;
    }
    int m,n;
    for(int i=0;i<G.arcnum;i++){
        for(m=0;m<G.vexnum;m++){
            if(G.vertices[m].data.key==VR[i][0])
            break;
        }
        for(n=0;n<G.vexnum;n++){
            if(G.vertices[n].data.key==VR[i][1])
            break;
        }
        if(m>=G.vexnum||n>=G.vexnum)  return ERROR;
        ArcNode *p=(ArcNode *)malloc(sizeof(ArcNode));
        p->adjvex=n;
        p->nextarc=G.vertices[m].firstarc;
        G.vertices[m].firstarc=p;
        p=(ArcNode *)malloc(sizeof(ArcNode));
        p->adjvex=m;
        p->nextarc=G.vertices[n].firstarc;
        G.vertices[n].firstarc=p;
    }
    return OK;
   

    /********** End **********/
}
status DestroyGraph(ALGraph &G)
/*销毁无向图G,删除G的全部顶点和边*/
{
    // 请在这里补充代码，完成本关任务
    /********** Begin *********/
    if (G.vexnum == 0 && G.arcnum == 0) {
        return OK;
    }
    for(int i=0;i<G.vexnum;i++){
        ArcNode *p=G.vertices[i].firstarc;
        while(p!=NULL){
            ArcNode *temp=p;
            p=p->nextarc;
            free(temp);
        }
        G.vertices[i].firstarc = NULL;
    }
    G.arcnum=0;
    G.vexnum=0;
    return OK;
    /********** End **********/
}
int LocateVex(ALGraph G,KeyType u)
//根据u在图G中查找顶点，查找成功返回位序，否则返回-1；
{
    // 请在这里补充代码，完成本关任务
    /********** Begin *********/
    for(int i=0;i<G.vexnum;i++){
        if(G.vertices[i].data.key==u) return i;
    }
    return -1;
    /********** End **********/
}
int isuniqueofG(ALGraph G){
    int i,j;
    for(i=0;i<G.vexnum;i++){
        for(j=i+1;j<G.vexnum;j++){
            if(G.vertices[i].data.key==G.vertices[j].data.key)  return 0;
        }
    }
    return 1;
}
status PutVex(ALGraph &G,KeyType u,VertexType value)
//根据u在图G中查找顶点，查找成功将该顶点值修改成value，返回OK；
//如果查找失败或关键字不唯一，返回ERROR
{
    // 请在这里补充代码，完成本关任务
    /********** Begin *********/
    int flag;
    if((flag=LocateVex(G,u))==-1||!isuniqueofG(G)) return ERROR;
    G.vertices[flag].data=value;
    if(!isuniqueofG(G))  return ERROR;
    return OK;
    /********** End **********/
}
int FirstAdjVex(ALGraph G,KeyType u)
//根据u在图G中查找顶点，查找成功返回顶点u的第一邻接顶点位序，否则返回-1；
{
    // 请在这里补充代码，完成本关任务
    /********** Begin *********/
    int i=LocateVex(G,u);
    if(i==-1)  return ERROR;
    if(G.vertices[i].firstarc==NULL)  return ERROR;
    return G.vertices[i].firstarc->adjvex;

    /********** End **********/
}
int NextAdjVex(ALGraph G, KeyType v, KeyType w)
       // v 对应 G 的一个顶点，w 对应 v 的邻接顶点；操作结果是返回 v 的（相对于 w）下一个邻接顶点的位序；
       // 如果 w 是最后一个邻接顶点，或 v、w 对应顶点不存在，则返回 -1。
       {
           int i = LocateVex(G, v); 
           if (i == -1) return -1;  
       
           ArcNode *p = G.vertices[i].firstarc;  
           while (p != NULL) {
               if (p->adjvex == LocateVex(G, w)) {  
                   if (p->nextarc != NULL) {  
                       return p->nextarc->adjvex;  
                   } else {
                       return -1;
                   }
               }
               p = p->nextarc;
           }
       
           return -1;
       }
       status InsertVex(ALGraph &G,VertexType v)
//在图G中插入顶点v，成功返回OK,否则返回ERROR
{
    // 请在这里补充代码，完成本关任务
    /********** Begin *********/
   G.vertices[G.vexnum].data=v;
   G.vertices[G.vexnum].firstarc=NULL;
   G.vexnum++;
   if(!isuniqueofG(G))  return ERROR;
   if(G.vexnum>MAX_VERTEX_NUM)  return ERROR;
   return OK;
    /********** End **********/
}
status DeleteVex(ALGraph &G, KeyType v) {
    int i = LocateVex(G, v);  
    if (i == -1) return ERROR;  
    if (G.vexnum == 1) return ERROR;  

  
    ArcNode *p = G.vertices[i].firstarc;
    while (p != NULL) {
        ArcNode *temp = p;
        p = p->nextarc;
        free(temp);
        G.arcnum--; 
    }

    for (int j = 0; j < G.vexnum; j++) {
        if (j == i) continue;  

        ArcNode *prev = NULL;
        ArcNode *curr = G.vertices[j].firstarc;
        
        while (curr != NULL) {
            if (curr->adjvex == i) {  
                if (prev == NULL) {
                    G.vertices[j].firstarc = curr->nextarc;  
                    free(curr);
                    curr = G.vertices[j].firstarc;
                } else {
                    prev->nextarc = curr->nextarc;  
                    free(curr);
                    curr = prev->nextarc;
                }
                
            } else {
                if (curr->adjvex > i) {
                    curr->adjvex--;  
                }
                prev = curr;
                curr = curr->nextarc;
            }
        }
    }

  
    for (int j = i; j < G.vexnum - 1; j++) {
        G.vertices[j] = G.vertices[j + 1];
    }

    G.vexnum--; 
    return OK;
}
int isuniquenew(ALGraph G,int i,int j){
    ArcNode *p=G.vertices[i].firstarc;
    while(p!=NULL){
        if(p->adjvex==j)  return 0;
        else{
            p=p->nextarc;
        }
    }
    return 1;
}
status InsertArc(ALGraph &G,KeyType v,KeyType w)
//在图G中增加弧<v,w>，成功返回OK,否则返回ERROR
{
    // 请在这里补充代码，完成本关任务
    /********** Begin *********/
    int m=LocateVex(G,v),n=LocateVex(G,w);
    if(m==-1||n==-1)  return ERROR;
    if(!isuniquenew(G,m,n))  return ERROR;
    ArcNode *p=(ArcNode *)malloc(sizeof(ArcNode));
    p->nextarc=G.vertices[m].firstarc;
    G.vertices[m].firstarc=p;
    p->adjvex=n;
    p=(ArcNode *)malloc(sizeof(ArcNode));
    p->nextarc=G.vertices[n].firstarc;
    G.vertices[n].firstarc=p;
    p->adjvex=m;
    G.arcnum++;
    return OK;
    /********** End **********/
}
status DeleteArc(ALGraph &G,KeyType v,KeyType w)
//在图G中删除弧<v,w>，成功返回OK,否则返回ERROR
{
    // 请在这里补充代码，完成本关任务
    /********** Begin *********/
    int m=LocateVex(G,v),n=LocateVex(G,w);
    int flag=0;
    if(m==-1||n==-1)  return ERROR;
    ArcNode *p=G.vertices[m].firstarc;
    ArcNode *last=NULL;
    while(p!=NULL){
        if(p->adjvex==n){
           if(last==NULL){
            G.vertices[m].firstarc=p->nextarc;
            free(p);
           }else{
            last->nextarc=p->nextarc;
            free(p);
           }
        flag=1;
        break;
        }else{
            last=p;
            p=p->nextarc;
        }
    }
    p=G.vertices[n].firstarc;
    last=NULL;
    while(p!=NULL){
        if(p->adjvex==m){
           if(last==NULL){
            G.vertices[n].firstarc=p->nextarc;
            free(p);
           }else{
            last->nextarc=p->nextarc;
            free(p);
           }
        flag=1;
        break;
        }else{
            last=p;
            p=p->nextarc;
        }
    }
    if(flag==0)  return ERROR;
    G.arcnum--;
    return OK;
    /********** End **********/
}

void DFS(ALGraph G, int i, bool visited[], void (*visit)(VertexType)) {
    visited[i] = true;
    visit(G.vertices[i].data);
    ArcNode *p = G.vertices[i].firstarc;
    while (p != NULL) {
        if (!visited[p->adjvex]) {
            DFS(G, p->adjvex, visited, visit);
        }
        p = p->nextarc;
    }
}
status DFSTraverse(ALGraph &G, KeyType startKey, void (*visit)(VertexType)) {
    if (G.vexnum == 0) return ERROR;

    bool visited[MAX_VERTEX_NUM] = {false};

    if (startKey != -1) { // 如果指定了起始顶点
        int startIndex = LocateVex(G, startKey);
        if (startIndex == -1) {
            printf("起始顶点 '%d' 不存在！\n", startKey);
            return ERROR;
        }
        DFS(G, startIndex, visited, visit);
    } else { // 如果未指定起始顶点，遍历整个图
        for (int i = 0; i < G.vexnum; i++) {
            if (!visited[i]) {
                DFS(G, i, visited, visit);
            }
        }
    }

    return OK;
}
status BFSTraverse(ALGraph &G, KeyType startKey, void (*visit)(VertexType)) {
    if (G.vexnum == 0) return ERROR;

    bool visited[MAX_VERTEX_NUM] = {false};
    int QUEUE[MAX_VERTEX_NUM];
    int head = 0, tail = 0;

    if (startKey != -1) { // 如果指定了起始顶点
        int startIndex = LocateVex(G, startKey);
        if (startIndex == -1) {
            printf("起始顶点 '%d' 不存在！\n", startKey);
            return ERROR;
        }
        visited[startIndex] = true;
        visit(G.vertices[startIndex].data);
        QUEUE[tail++] = startIndex;

        while (head != tail) {
            int current = QUEUE[head++];
            ArcNode *p = G.vertices[current].firstarc;
            while (p != NULL) {
                if (!visited[p->adjvex]) {
                    visited[p->adjvex] = true;
                    visit(G.vertices[p->adjvex].data);
                    QUEUE[tail++] = p->adjvex;
                }
                p = p->nextarc;
            }
        }
    } else { // 如果未指定起始顶点，遍历整个图
        for (int v = 0; v < G.vexnum; v++) {
            if (!visited[v]) {
                visited[v] = true;
                visit(G.vertices[v].data);
                QUEUE[tail++] = v;

                while (head != tail) {
                    int current = QUEUE[head++];
                    ArcNode *p = G.vertices[current].firstarc;
                    while (p != NULL) {
                        if (!visited[p->adjvex]) {
                            visited[p->adjvex] = true;
                            visit(G.vertices[p->adjvex].data);
                            QUEUE[tail++] = p->adjvex;
                        }
                        p = p->nextarc;
                    }
                }
            }
        }
    }

    return OK;
}
std::vector<int> VerticesSetLessThanK(ALGraph &G, KeyType v, int k) {
    std::vector<int> result; // 存储结果集合
    if (k <= 0) return result; // 如果 k <= 0，直接返回空集合

    int start = LocateVex(G, v); // 查找顶点 v 的位置
    if (start == -1) return result; // 如果顶点 v 不存在，返回空集合

    std::vector<bool> visited(G.vexnum, false); // 记录顶点是否访问过
    std::vector<int> distance(G.vexnum, 0); // 记录每个顶点到 v 的距离
    std::queue<int> q; // 队列用于广度优先搜索

    visited[start] = true;
    q.push(start);

    while (!q.empty()) {
        int current = q.front();
        q.pop();

        ArcNode *p = G.vertices[current].firstarc;
        while (p != NULL) {
            int adj = p->adjvex;
            if (!visited[adj]) {
                visited[adj] = true;
                distance[adj] = distance[current] + 1;
                if (distance[adj] < k) {
                    result.push_back(G.vertices[adj].data.key); // 将顶点关键字加入结果集合
                    q.push(adj);
                }
            }
            p = p->nextarc;
        }
    }

    return result;
}
int ShortestPathLength(ALGraph &G, KeyType v, KeyType w) {
    int start = LocateVex(G, v); // 查找顶点 v 的位置
    int end = LocateVex(G, w);  // 查找顶点 w 的位置

    if (start == -1 || end == -1) {
        return -1; // 如果顶点 v 或 w 不存在，返回 -1 表示不存在路径
    }

    if (start == end) {
        return 0; // 如果起点和终点相同，最短路径长度为 0
    }

    std::vector<bool> visited(G.vexnum, false); // 记录顶点是否访问过
    std::vector<int> distance(G.vexnum, 0);    // 记录每个顶点到起点的距离
    std::queue<int> q;                         // 队列用于广度优先搜索

    visited[start] = true;
    q.push(start);

    while (!q.empty()) {
        int current = q.front();
        q.pop();

        ArcNode *p = G.vertices[current].firstarc;
        while (p != NULL) {
            int adj = p->adjvex;
            if (!visited[adj]) {
                visited[adj] = true;
                distance[adj] = distance[current] + 1;

                if (adj == end) {
                    return distance[adj]; // 找到目标顶点，返回最短路径长度
                }

                q.push(adj);
            }
            p = p->nextarc;
        }
    }

    return -1; // 如果无法到达目标顶点，返回 -1 表示不存在路径
}
int ConnectedComponentsNums(ALGraph &G) {
    if (G.vexnum == 0) {
        return 0; // 如果图中没有顶点，连通分量为 0
    }

    std::vector<bool> visited(G.vexnum, false); // 记录顶点是否访问过
    int componentCount = 0; // 连通分量计数

    // 深度优先搜索辅助函数
    auto DFS = [&](int v, auto &DFS) -> void {
        visited[v] = true;
        ArcNode *p = G.vertices[v].firstarc;
        while (p != NULL) {
            if (!visited[p->adjvex]) {
                DFS(p->adjvex, DFS);
            }
            p = p->nextarc;
        }
    };

    // 遍历所有顶点，查找未访问的顶点并进行 DFS
    for (int i = 0; i < G.vexnum; i++) {
        if (!visited[i]) {
            componentCount++; // 每次发现一个未访问的顶点，连通分量加 1
            DFS(i, DFS);
        }
    }

    return componentCount;
}
status SaveGraph(ALGraph G, char FileName[])
      
       {
           FILE *fp = fopen(FileName, "w");
           if (fp == NULL) return ERROR;
       
          
           fprintf(fp, "%d %d\n", G.vexnum, G.arcnum);
       
         
           for (int i = 0; i < G.vexnum; i++) {
               fprintf(fp, "%d %s\n", G.vertices[i].data.key, G.vertices[i].data.others);
           }
       
          
           for (int i = 0; i < G.vexnum; i++) {
               ArcNode *p = G.vertices[i].firstarc;
               while (p != NULL) {
                   fprintf(fp, "%d %d\n", i, p->adjvex);
                   p = p->nextarc;
               }
           }
       
           fclose(fp);
           return OK;
       }
       
       status LoadGraph(ALGraph &G, char FileName[])
    
       {
           FILE *fp = fopen(FileName, "r");
           if (fp == NULL) return ERROR;
       
    
           fscanf(fp, "%d %d", &G.vexnum, &G.arcnum);
       
     
           for (int i = 0; i < G.vexnum; i++) {
               fscanf(fp, "%d %s", &G.vertices[i].data.key, G.vertices[i].data.others);
               G.vertices[i].firstarc = NULL; 
           }
       
           
           int from, to;
           while (fscanf(fp, "%d %d", &from, &to) != EOF) {
            ArcNode *newNode = (ArcNode *)malloc(sizeof(ArcNode));
            newNode->adjvex = to;
            newNode->nextarc = NULL; 
        
            if (G.vertices[from].firstarc == NULL) {
                
                G.vertices[from].firstarc = newNode;
            } else {
                
                ArcNode *tail = G.vertices[from].firstarc;
                while (tail->nextarc != NULL) {
                    tail = tail->nextarc; 
                }
                tail->nextarc = newNode; 
            }
        }
       
           fclose(fp);
           return OK;
       }
