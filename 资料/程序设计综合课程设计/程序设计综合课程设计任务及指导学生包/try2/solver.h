#ifndef Solver_h
#define Solver_h

#include "cnfparser.h"

#include<time.h>
#include<math.h>
#include<stdlib.h>
#include<string.h>

/**
 * @brief DPLL算法递归入口
 * 
 * @param cnf传递指针p_cnf 
 * @return 是否解决布尔值int 
 */
int dpll_solve(cnf* p_cnf);

/**
 * @brief 保存求解结果到文件filename
 * 
 * @param result 
 * @param time 
 * @param p_cnf 
 * @param filename 
 */
void save_result(int result, clock_t time, cnf *p_cnf, const char *filename);

/**
 * @brief DPLL递归函数
 *
 * @param cnf传递指针p_cnf
 * @return 是否解决布尔值int
 */
int dpll_recursion(cnf* p_cnf);

/**
 * @brief 查找单子句
 *
 * @param cnf传递指针p_cnf
 * @return 单子句的name * flag
 */
int find_unit_clause(cnf *p_cnf);

/**
 * @brief Q-DPLL算法求解入口
 * 
 * @param cnf传递指针p_cnf 
 * @return 是否解决布尔值int 
 */
int q_dpll_solve(cnf* p_cnf);

/*
 * @brief Q-DPLL算法递归函数
 *
 * @param cnf传递指针p_cnf
 * @return 是否解决布尔值int
 */
int q_dpll_recursion(cnf* p_cnf);

/*
 * @brief 选择变量启发式
 *
 * @param cnf传递指针p_cnf
 * @return 选择的变量索引
 */
int q_find_fun(cnf* p_cnf);
#endif