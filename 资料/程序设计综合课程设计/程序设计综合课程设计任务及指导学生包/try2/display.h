#ifndef Display_h
#define Display_h

#include <stdio.h>
#include <time.h>
#include <dirent.h>
#include <sys/stat.h>
#include "cnfparser.h"
#include "solver.h"

// 主菜单
void print_menu(void);

// 次级菜单
void print_sec_menu(void);

// 输出结果
void print_result(int result, clock_t time, cnf *p_cnf);

// 优化率
void print_improve(clock_t whole_time, clock_t q_whole_time, int result);

//输出子句
void print_clause(cnf *p_cnf);

// 显示CNF文件列表并选择
int select_cnf_file(char *selected_filename);

#endif