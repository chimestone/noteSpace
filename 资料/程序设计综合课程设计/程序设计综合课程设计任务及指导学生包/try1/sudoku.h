#ifndef Sudoku_h
#define Sudoku_h

#include "cnfparser.h"
#include "solver.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <direct.h>
#include <dirent.h>
#include <sys/stat.h>

// 百分号数独菜单
void percent_sudoku_menu();

// 生成百分号数独
void generate_percent_sudoku();

// 游戏百分号数独（使用DPLL求解）
void play_percent_sudoku();

// 生成完整的百分号数独解
int generate_complete_sudoku(int grid[9][9]);

// 挖空并验证唯一解
void dig_holes(int grid[9][9], int holes);

// 验证百分号数独规则
int validate_percent_sudoku_move(int grid[9][9], int row, int col, int num);

// 回溯求解百分号数独
int solve_percent_sudoku_backtrack(int grid[9][9]);

// 验证百分号数独完整性
int is_valid_percent_sudoku(int grid[9][9]);

// 将数独转换为CNF格式
void sudoku_to_cnf(int grid[9][9], const char* filename);

// 从DPLL解析数独
void parse_sudoku_from_solution(int* solution, int grid[9][9]);

// 显示数独
void display_sudoku(int grid[9][9]);

// 检查是否完成
int is_sudoku_complete(int grid[9][9]);

// 随机化回溯求解（仅用于生成）
int solve_percent_sudoku_random(int grid[9][9]);

// 随机化数组
void shuffle_array(int arr[], int n);

// 选择数独CNF文件
int select_sudoku_file(char *selected_filename);

// 调试函数：验证数独完整性
void debug_sudoku(int grid[9][9]);

#endif