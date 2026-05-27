/*任务描述
本关任务：假设称正读和反读都相同的字符序列为“回文”，例如，‘abba’和‘abcba’ 是回文，‘abcde’和‘ababab’则不是回文。试写一个算法判别读入的一个以‘@’为 结束符的字符序列是否是“回文”。 

相关知识
为了完成本关任务，你需要掌握：1.栈及其运算，2.队列及其运算。

编程要求
要求必须利用已通过的第1关栈的函数和第2关队列的函数，实现本关的任务。本关测试程序中已将前2关的函数包含在本关中，可以直接按函数名调用，如果前2关未通过，未按要求完成本关题目得分不超过本题分数的50%。
根据提示，在右侧编辑器完成函数：int isPalindrome(char *str) 的定义，满足任务要求。

测试说明
平台会自动读取输入数据，对你编写的代码进行测试，并输出结果。

测试输入：abcdedcba@
预期输出：YES

测试输入：1234567@
预期输出：NO*/
#include "stdio.h"
typedef char ElemType;
#define MAXLENGTH 100
#define N 100
typedef struct TWSTACK {
     ElemType elem[N];
     int top1,top2;
     } TWSTACK;
typedef struct QUEUE {
     ElemType elem[MAXLENGTH];
     int front,length;
     } QUEUE;

//涉及函数
//在下面的begin和end间填写相应代码
void inistack(TWSTACK &S)
//该函数实现初始化S，得到2个空栈。根据双向栈的示意图，理解初始化要求。
{
/***************begin***************/
   S.top1=0;
   S.top2=N-1;
/*************** end ***************/
}
int push(TWSTACK &S,int i,ElemType e)
//i取值1或2，分别对应左或右栈，将元素e压入S的对应栈。成功入栈返回1，否则返回0
{
/***************begin***************/
   if (S.top1 > S.top2) return 0;
   switch (i) {
      case 1:
         S.top1++;
         S.elem[S.top1-1]=e;
         return 1;
      case 2:
         S.top2--;
         S.elem[S.top2+1]=e;
         return 1;
      default : return 0;
   }
/*************** end ***************/
}
int pop(TWSTACK &S,int i, ElemType &e)
//i取值1或2，分别对应左或右栈，将S对应栈的栈顶元素出栈，赋值给e。成功出栈返回1，否则返回0
{
/***************begin***************/
   switch (i) {
      case 1:
         if (S.top1<=0) return 0;
         e=S.elem[S.top1-1];
         S.top1--;
         return 1;
      case 2:
         if (S.top2>=N-1) return 0;
         e=S.elem[S.top2+1];
         S.top2++;
         return 1;
      default : return 0;
   }

/*************** end ***************/
}

//在下面的begin和end间填写相应代码
void iniQueue(QUEUE &Q)
//该函数实现初始化Q
{
/***************begin***************/
    Q.front=0;
    Q.length=0;

/*************** end ***************/
}
int enQueue(QUEUE &Q,ElemType e)
//将元素e入队Q。成功入栈返回1，否则返回0
{
/***************begin***************/
    if (Q.length >= MAXLENGTH) return 0;
    Q.elem[(Q.front+Q.length)%MAXLENGTH]=e;
    Q.length++;
    return 1;


/*************** end ***************/
}
int deQueue(QUEUE &Q, ElemType &e)
//将Q队首元素出队，赋值给e。成功出队返回1，否则返回0
{
/***************begin***************/
    if (Q.length == 0) return 0;
    e=Q.elem[Q.front];
    Q.front++;
    Q.length--;
    if (Q.front>=MAXLENGTH) Q.front%=MAXLENGTH;
    return 1;

/*************** end ***************/
}

int isPalindrome(char *str);


int main()
{
    char str[100];
    scanf("%s",&str);
    printf("%s",isPalindrome(str)?"YES":"NO");
    return 0;
}
int isPalindrome(char *str)
//判断字符串str是否回文，是则返回1，否则返回0
{
/**********begin**********/
    int length=0;
    char check1, check2;
    for(int i=0; str[i]!='@'; i++) length++;
    TWSTACK S;
    QUEUE Q;
    inistack(S);
    iniQueue(Q);
    for (int i=0; i<length; i++) {
        push(S,1,str[i]);
        enQueue(Q,str[i]);
    }
    for(int i=0; i<length; i++) {
        pop(S,1,check1);
        deQueue(Q,check2);
        if (check1 != check2) return 0;
    }
    return 1;
/********** end **********/
}

