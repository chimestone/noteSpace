#ifndef Cnfparser_h
#define Cnfparser_h

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

// 文字结构体
typedef struct literal
{
    int name;             // 变量名
    int flag;             // 符号
    struct literal *next; // 次级指针
} literal;

// 子句结构体
typedef struct clause
{
    literal *first_literal; // 首文字指针
    int size;               // 子句长度
    struct clause *next;    // 次级子句
} clause;

// CNF范式结构体
typedef struct cnf
{
    clause *first_clause; // 首子句指针
    int num_literal;      // 总文字数
    int num_clause;       // 总子句数
    int *arr;             // 记忆数组
} cnf;

/**
 * @brief 获取cnf文件
 *
 * @param cnf文件名称filename
 * @return 获取到的cnf结构体指针
 * @return 获取失败为NULL
 */
cnf *obtain_cnf(const char *filename);

/**
 * @brief 释放存储空间
 *
 * @param 要释放空间的cnf结构体指针p_cnf
 */
void free_cnf(cnf *p_cnf);

/**
 * @brief 计算子句在arr赋值条件下的真假值
 *
 * @param 子句指针p_clause
 * @param 记忆数组arr
 * @return int真假值
 */
int evaluate_clause(clause *p_clause, int *arr);

#endif