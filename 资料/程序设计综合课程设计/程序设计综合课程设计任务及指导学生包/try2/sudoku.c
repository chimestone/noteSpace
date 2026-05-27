#include "sudoku.h"

void percent_sudoku_menu()
{
    int choice;
    while (1)
    {
        printf("\n=== Percent Sudoku Game ===\n");
        printf("1. Generate Percent Sudoku\n");
        printf("2. Play Percent Sudoku\n");
        printf("3. Back to Main Menu\n");
        printf("Enter your choice: ");

        scanf("%d", &choice);

        switch (choice)
        {
        case 1:
            generate_percent_sudoku();
            break;
        case 2:
            play_percent_sudoku();
            break;
        case 3:
            return;
        default:
            printf("Invalid choice!\n");
        }
    }
}

void generate_percent_sudoku()
{
    int grid[9][9] = {0};
    int holes;
    char save_choice;

    printf("\nGenerating Percent Sudoku...\n");

    if (!generate_complete_sudoku(grid))
    {
        printf("Generation failed!\n");
        return;
    }

    printf("Enter number of holes to dig (1-64): ");
    scanf("%d", &holes);

    if (holes < 1 || holes > 64)
    {
        printf("Invalid number of holes!\n");
        return;
    }

    dig_holes(grid, holes);

    printf("\nGenerated Percent Sudoku:\n");
    display_sudoku(grid);
    
    // 验证生成的数独是否满足百分号约束（只检查非空格子）
    int temp_grid[9][9];
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            temp_grid[i][j] = grid[i][j];
        }
    }
    
    // 临时填满空格子进行完整性验证
    if (solve_percent_sudoku_backtrack(temp_grid)) {
        if (is_valid_percent_sudoku(temp_grid)) {
            printf("Generated sudoku passes all percent constraints!\n");
        } else {
            printf("WARNING: Generated sudoku violates percent constraints!\n");
        }
    } else {
        printf("WARNING: Generated sudoku has no valid solution!\n");
    }
    
    // 调试信息
    debug_sudoku(grid);
    
    // 验证生成的数独
    int empty_count = 0;
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            if (grid[i][j] == 0) empty_count++;
        }
    }
    printf("Empty cells: %d\n", empty_count);

    printf("\nSave as CNF file? (y/n): ");
    scanf(" %c", &save_choice);

    if (save_choice == 'y' || save_choice == 'Y')
    {
        _mkdir("sudoku");

        char user_filename[100];
        char full_filename[150];
        printf("Enter filename (without .cnf extension): ");
        scanf("%s", user_filename);
        sprintf(full_filename, "sudoku/%s.cnf", user_filename);

        // 在保存为CNF之前，再次验证数独的可解性
        int validation_grid[9][9];
        for (int i = 0; i < 9; i++) {
            for (int j = 0; j < 9; j++) {
                validation_grid[i][j] = grid[i][j];
            }
        }
        
        if (solve_percent_sudoku_backtrack(validation_grid)) {
            sudoku_to_cnf(grid, full_filename);
            printf("Saved to %s\n", full_filename);
            
            // 立即测试CNF是否可解
            printf("Testing CNF solvability...\n");
            cnf *test_cnf = obtain_cnf(full_filename);
            if (test_cnf) {
                if (dpll_solve(test_cnf)) {
                    printf("CNF is solvable - good!\n");
                } else {
                    printf("WARNING: Generated CNF is not solvable!\n");
                }
                free_cnf(test_cnf);
            }
        } else {
            printf("ERROR: Generated sudoku is not solvable! Not saving CNF.\n");
        }
    }
}

void play_percent_sudoku()
{
    char filename[256];
    int puzzle[9][9] = {0};    // 题目（只有提示数字）
    int solution[9][9] = {0};  // DPLL求解的标准答案
    int user_grid[9][9] = {0}; // 用户当前填入的数字
    int x, y, num;

    // 选择CNF文件
    if (!select_sudoku_file(filename)) {
        printf("No file selected.\n");
        return;
    }

    // 使用DPLL求解CNF文件获得标准答案
    cnf *p_cnf = obtain_cnf(filename);
    if (!p_cnf) {
        printf("Failed to load CNF file!\n");
        return;
    }

    printf("Solving sudoku with DPLL...\n");
    printf("CNF has %d variables and %d clauses\n", p_cnf->num_literal, p_cnf->num_clause);
    
    if (!dpll_solve(p_cnf)) {
        printf("No solution found!\n");
        
        // 输出一些调试信息
        printf("Debug: Checking if CNF is satisfiable...\n");
        
        free_cnf(p_cnf);
        return;
    } else {
        printf("Solution found!\n");
    }

    // 从DPLL解析标准答案
    parse_sudoku_from_solution(p_cnf->arr, solution);

    // 从CNF文件解析题目（提示数字）
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Cannot read file!\n");
        free_cnf(p_cnf);
        return;
    }

    char line[1024];
    while (fgets(line, sizeof(line), file)) {
        if (line[0] == 'c' || line[0] == 'p') continue;
        
        char *token = strtok(line, " \t\n");
        while (token != NULL && atoi(token) != 0) {
            int var = atoi(token);
            if (var > 0 && var <= 729) {
                int i = (var - 1) / 81;
                int j = ((var - 1) % 81) / 9;
                int k = ((var - 1) % 9) + 1;

                // 检查是否为单子句（提示数字）
                char *next_token = strtok(NULL, " \t\n");
                if (next_token == NULL || atoi(next_token) == 0) {
                    puzzle[i][j] = k;
                    user_grid[i][j] = k;
                }
                break;
            }
            token = strtok(NULL, " \t\n");
        }
    }
    fclose(file);

    printf("\nSudoku puzzle (hints shown):\n");
    display_sudoku(user_grid);

    // 游戏循环
    while (!is_sudoku_complete(user_grid)) {
        printf("\nEnter your move (x y num) or (0 0 0) to quit: ");
        scanf("%d %d %d", &x, &y, &num);

        if (x == 0 && y == 0 && num == 0) break;

        if (x < 1 || x > 9 || y < 1 || y > 9 || num < 1 || num > 9) {
            printf("Invalid input range!\n");
            continue;
        }

        x--; y--; // 转换为0-8索引

        if (puzzle[x][y] != 0) {
            printf("Position is a hint and cannot be changed!\n");
            continue;
        }

        if (user_grid[x][y] != 0) {
            printf("Position already filled! Enter 0 to clear: ");
            int clear;
            scanf("%d", &clear);
            if (clear == 0) {
                user_grid[x][y] = 0;
                display_sudoku(user_grid);
            }
            continue;
        }

        // 与标准答案对比
        if (solution[x][y] == num) {
            user_grid[x][y] = num;
            printf("Correct!\n");
            display_sudoku(user_grid);
        } else {
            printf("Wrong! The correct answer is %d\n", solution[x][y]);
        }
    }

    if (is_sudoku_complete(user_grid)) {
        printf("\nCongratulations! Sudoku completed!\n");
    }

    free_cnf(p_cnf);
}

int generate_complete_sudoku(int grid[9][9])
{
    static int first_call = 1;
    
    if (first_call) {
        srand((unsigned int)time(NULL));
        first_call = 0;
    }

    // 清空网格
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            grid[i][j] = 0;
        }
    }
    
    // 使用随机回溯算法生成满足百分号数独约束的完整解
    printf("Generating complete percent sudoku...\n");
    if (solve_percent_sudoku_random(grid)) {
        printf("Generation successful!\n");
        return 1;
    } else {
        printf("Generation failed, trying again...\n");
        // 如果失败，再试一次
        for (int i = 0; i < 9; i++) {
            for (int j = 0; j < 9; j++) {
                grid[i][j] = 0;
            }
        }
        return solve_percent_sudoku_random(grid);
    }
}

void dig_holes(int grid[9][9], int holes)
{
    int dug = 0;
    int attempts = 0;
    const int max_attempts = holes * 20; // 增加尝试次数
    
    while (dug < holes && attempts < max_attempts) {
        attempts++;
        int row = rand() % 9;
        int col = rand() % 9;
        
        if (grid[row][col] != 0) {
            int backup = grid[row][col];
            grid[row][col] = 0;
            
            // 检查挖空后是否仍然有唯一解
            int temp_grid1[9][9], temp_grid2[9][9];
            for (int i = 0; i < 9; i++) {
                for (int j = 0; j < 9; j++) {
                    temp_grid1[i][j] = grid[i][j];
                    temp_grid2[i][j] = grid[i][j];
                }
            }
            
            // 第一次求解
            int has_solution1 = solve_percent_sudoku_backtrack(temp_grid1);
            
            if (has_solution1) {
                // 检查是否有多个解（简化检查）
                // 在空格子中尝试不同的值
                int unique = 1;
                for (int test_val = 1; test_val <= 9 && unique; test_val++) {
                    if (validate_percent_sudoku_move(grid, row, col, test_val)) {
                        temp_grid2[row][col] = test_val;
                        if (solve_percent_sudoku_backtrack(temp_grid2)) {
                            // 检查是否与第一个解不同
                            int different = 0;
                            for (int i = 0; i < 9 && !different; i++) {
                                for (int j = 0; j < 9 && !different; j++) {
                                    if (temp_grid1[i][j] != temp_grid2[i][j]) {
                                        different = 1;
                                    }
                                }
                            }
                            if (different) {
                                unique = 0; // 找到了不同的解
                            }
                        }
                        // 重置用于下一次测试
                        for (int i = 0; i < 9; i++) {
                            for (int j = 0; j < 9; j++) {
                                temp_grid2[i][j] = grid[i][j];
                            }
                        }
                    }
                }
                
                if (unique) {
                    dug++; // 有唯一解，可以挖空
                    printf("Dug hole at (%d,%d), progress: %d/%d\n", row+1, col+1, dug, holes);
                } else {
                    grid[row][col] = backup; // 非唯一解，恢复
                }
            } else {
                grid[row][col] = backup; // 无解，恢复
            }
        }
    }
    
    if (dug < holes) {
        printf("Warning: Only dug %d holes out of %d requested\n", dug, holes);
    }
}

int solve_percent_sudoku_backtrack(int grid[9][9])
{
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            if (grid[i][j] == 0) {
                for (int num = 1; num <= 9; num++) {
                    if (validate_percent_sudoku_move(grid, i, j, num)) {
                        grid[i][j] = num;
                        if (solve_percent_sudoku_backtrack(grid)) {
                            return 1;
                        }
                        grid[i][j] = 0;
                    }
                }
                return 0;
            }
        }
    }
    return 1;
}

int is_valid_percent_sudoku(int grid[9][9])
{
    // 检查每个非空格子是否符合所有约束
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            if (grid[i][j] != 0) {
                int num = grid[i][j];
                grid[i][j] = 0; // 临时移除该数字
                if (!validate_percent_sudoku_move(grid, i, j, num)) {
                    grid[i][j] = num; // 恢复
                    printf("Invalid sudoku at position (%d,%d) with value %d\n", i+1, j+1, num);
                    return 0;
                }
                grid[i][j] = num; // 恢复
            }
        }
    }
    
    return 1; // 只检查约束，不要求所有格子都填满
}

void shuffle_array(int arr[], int n)
{
    for (int i = n - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int temp = arr[i];
        arr[i] = arr[j];
        arr[j] = temp;
    }
}

int solve_percent_sudoku_random(int grid[9][9])
{
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            if (grid[i][j] == 0) {
                int nums[9] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
                shuffle_array(nums, 9);

                for (int k = 0; k < 9; k++) {
                    int num = nums[k];
                    if (validate_percent_sudoku_move(grid, i, j, num)) {
                        grid[i][j] = num;
                        if (solve_percent_sudoku_random(grid)) {
                            return 1;
                        }
                        grid[i][j] = 0;
                    }
                }
                return 0;
            }
        }
    }
    
    // 最终验证生成的数独是否完全有效
    return is_valid_percent_sudoku(grid);
}

int validate_percent_sudoku_move(int grid[9][9], int row, int col, int num)
{
    // 检查行
    for (int j = 0; j < 9; j++) {
        if (j != col && grid[row][j] == num) return 0;
    }

    // 检查列
    for (int i = 0; i < 9; i++) {
        if (i != row && grid[i][col] == num) return 0;
    }

    // 检查3x3盒子
    int box_row = (row / 3) * 3;
    int box_col = (col / 3) * 3;
    for (int i = box_row; i < box_row + 3; i++) {
        for (int j = box_col; j < box_col + 3; j++) {
            if ((i != row || j != col) && grid[i][j] == num) return 0;
        }
    }

    // 检查反对角线约束
    int diag_pos[9][2] = {{0, 8}, {1, 7}, {2, 6}, {3, 5}, {4, 4}, {5, 3}, {6, 2}, {7, 1}, {8, 0}};
    for (int p = 0; p < 9; p++) {
        if (diag_pos[p][0] == row && diag_pos[p][1] == col) {
            for (int q = 0; q < 9; q++) {
                if (q != p && grid[diag_pos[q][0]][diag_pos[q][1]] == num) return 0;
            }
            break;
        }
    }

    // 检查窗口1约束 (1,1)-(3,3) - 注意这里是0-基索引
    if (row >= 1 && row <= 3 && col >= 1 && col <= 3) {
        for (int i = 1; i <= 3; i++) {
            for (int j = 1; j <= 3; j++) {
                if ((i != row || j != col) && grid[i][j] == num) return 0;
            }
        }
    }

    // 检查窗口2约束 (5,5)-(7,7) - 注意这里是0-基索引
    if (row >= 5 && row <= 7 && col >= 5 && col <= 7) {
        for (int i = 5; i <= 7; i++) {
            for (int j = 5; j <= 7; j++) {
                if ((i != row || j != col) && grid[i][j] == num) return 0;
            }
        }
    }

    return 1;
}

void sudoku_to_cnf(int grid[9][9], const char *filename)
{
    FILE *file = fopen(filename, "w");
    if (!file) return;

    int hints = 0;
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            if (grid[i][j] != 0) hints++;
        }
    }

    // 计算子句数量：格约束 + 行约束 + 列约束 + 盒子约束 + 反对角线约束 + 窗口约束 + 提示
    int clause_count = 81 + 81*36 + 9*9 + 9*36 + 9*9 + 9*36 + 9*9 + 9*36 + 9 + 9*36 + 9 + 9*36 + 9 + 9*36 + hints;
    fprintf(file, "p cnf 729 %d\n", clause_count);

    // 1. 格约束
    for (int i = 1; i <= 9; i++) {
        for (int j = 1; j <= 9; j++) {
            for (int k = 1; k <= 9; k++) {
                fprintf(file, "%d ", (i - 1) * 81 + (j - 1) * 9 + k);
            }
            fprintf(file, "0\n");

            for (int k1 = 1; k1 <= 9; k1++) {
                for (int k2 = k1 + 1; k2 <= 9; k2++) {
                    fprintf(file, "-%d -%d 0\n",
                            (i - 1) * 81 + (j - 1) * 9 + k1,
                            (i - 1) * 81 + (j - 1) * 9 + k2);
                }
            }
        }
    }

    // 2. 行约束
    for (int i = 1; i <= 9; i++) {
        for (int k = 1; k <= 9; k++) {
            for (int j = 1; j <= 9; j++) {
                fprintf(file, "%d ", (i - 1) * 81 + (j - 1) * 9 + k);
            }
            fprintf(file, "0\n");

            for (int j1 = 1; j1 <= 9; j1++) {
                for (int j2 = j1 + 1; j2 <= 9; j2++) {
                    fprintf(file, "-%d -%d 0\n",
                            (i - 1) * 81 + (j1 - 1) * 9 + k,
                            (i - 1) * 81 + (j2 - 1) * 9 + k);
                }
            }
        }
    }

    // 3. 列约束
    for (int j = 1; j <= 9; j++) {
        for (int k = 1; k <= 9; k++) {
            for (int i = 1; i <= 9; i++) {
                fprintf(file, "%d ", (i - 1) * 81 + (j - 1) * 9 + k);
            }
            fprintf(file, "0\n");

            for (int i1 = 1; i1 <= 9; i1++) {
                for (int i2 = i1 + 1; i2 <= 9; i2++) {
                    fprintf(file, "-%d -%d 0\n",
                            (i1 - 1) * 81 + (j - 1) * 9 + k,
                            (i2 - 1) * 81 + (j - 1) * 9 + k);
                }
            }
        }
    }

    // 4. 3x3盒子约束
    for (int box = 0; box < 9; box++) {
        int start_row = (box / 3) * 3;
        int start_col = (box % 3) * 3;

        for (int k = 1; k <= 9; k++) {
            // 每个盒子包含每个数字
            for (int i = start_row; i < start_row + 3; i++) {
                for (int j = start_col; j < start_col + 3; j++) {
                    fprintf(file, "%d ", i * 81 + j * 9 + k);
                }
            }
            fprintf(file, "0\n");
            
            // 每个盒子中每个数字只出现一次
            for (int pos1 = 0; pos1 < 9; pos1++) {
                for (int pos2 = pos1 + 1; pos2 < 9; pos2++) {
                    int i1 = start_row + pos1 / 3;
                    int j1 = start_col + pos1 % 3;
                    int i2 = start_row + pos2 / 3;
                    int j2 = start_col + pos2 % 3;
                    fprintf(file, "-%d -%d 0\n",
                            i1 * 81 + j1 * 9 + k,
                            i2 * 81 + j2 * 9 + k);
                }
            }
        }
    }

    // 5. 百分号约束：反对角线 (0,8)-(8,0)
    int diag_pos[9][2] = {{0, 8}, {1, 7}, {2, 6}, {3, 5}, {4, 4}, {5, 3}, {6, 2}, {7, 1}, {8, 0}};
    for (int k = 1; k <= 9; k++) {
        // 反对角线包含每个数字
        for (int p = 0; p < 9; p++) {
            int var = diag_pos[p][0] * 81 + diag_pos[p][1] * 9 + k;
            fprintf(file, "%d ", var);
        }
        fprintf(file, "0\n");
        
        // 反对角线每个数字只出现一次
        for (int p1 = 0; p1 < 9; p1++) {
            for (int p2 = p1 + 1; p2 < 9; p2++) {
                int var1 = diag_pos[p1][0] * 81 + diag_pos[p1][1] * 9 + k;
                int var2 = diag_pos[p2][0] * 81 + diag_pos[p2][1] * 9 + k;
                fprintf(file, "-%d -%d 0\n", var1, var2);
            }
        }
    }

    // 6. 百分号约束：窗口1 (1,1)-(3,3)
    for (int k = 1; k <= 9; k++) {
        // 窗口1包含每个数字
        for (int i = 1; i <= 3; i++) {
            for (int j = 1; j <= 3; j++) {
                int var = i * 81 + j * 9 + k;
                fprintf(file, "%d ", var);
            }
        }
        fprintf(file, "0\n");
        
        // 窗口1每个数字只出现一次
        for (int pos1 = 0; pos1 < 9; pos1++) {
            for (int pos2 = pos1 + 1; pos2 < 9; pos2++) {
                int i1 = 1 + pos1 / 3;
                int j1 = 1 + pos1 % 3;
                int i2 = 1 + pos2 / 3;
                int j2 = 1 + pos2 % 3;
                int var1 = i1 * 81 + j1 * 9 + k;
                int var2 = i2 * 81 + j2 * 9 + k;
                fprintf(file, "-%d -%d 0\n", var1, var2);
            }
        }
    }

    // 7. 百分号约束：窗口2 (5,5)-(7,7)
    for (int k = 1; k <= 9; k++) {
        // 窗口2包含每个数字
        for (int i = 5; i <= 7; i++) {
            for (int j = 5; j <= 7; j++) {
                int var = i * 81 + j * 9 + k;
                fprintf(file, "%d ", var);
            }
        }
        fprintf(file, "0\n");
        
        // 窗口2每个数字只出现一次
        for (int pos1 = 0; pos1 < 9; pos1++) {
            for (int pos2 = pos1 + 1; pos2 < 9; pos2++) {
                int i1 = 5 + pos1 / 3;
                int j1 = 5 + pos1 % 3;
                int i2 = 5 + pos2 / 3;
                int j2 = 5 + pos2 % 3;
                int var1 = i1 * 81 + j1 * 9 + k;
                int var2 = i2 * 81 + j2 * 9 + k;
                fprintf(file, "-%d -%d 0\n", var1, var2);
            }
        }
    }

    // 8. 提示数字（只为非空格子添加约束）
    int hint_count = 0;
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            if (grid[i][j] != 0) {
                fprintf(file, "%d 0\n", i * 81 + j * 9 + grid[i][j]);
                hint_count++;
            }
        }
    }
    printf("Added %d hint constraints to CNF\n", hint_count);

    fclose(file);
}

void parse_sudoku_from_solution(int *solution, int grid[9][9])
{
    // 初始化网格
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            grid[i][j] = 0;
        }
    }
    
    // 从解中提取数独
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            for (int k = 1; k <= 9; k++) {
                int var = i * 81 + j * 9 + k;
                if (var >= 1 && var <= 729 && solution[var] == 1) {
                    grid[i][j] = k;
                    break;
                }
            }
        }
    }
}

void display_sudoku(int grid[9][9])
{
    printf("\n  1 2 3   4 5 6   7 8 9\n");
    for (int i = 0; i < 9; i++) {
        if (i % 3 == 0 && i != 0) {
            printf("  ------+-------+------\n");
        }
        printf("%d ", i + 1);
        for (int j = 0; j < 9; j++) {
            if (j % 3 == 0 && j != 0) printf("| ");
            if (grid[i][j] == 0) {
                printf(". ");
            } else {
                printf("%d ", grid[i][j]);
            }
        }
        printf("\n");
    }
}

int is_sudoku_complete(int grid[9][9])
{
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            if (grid[i][j] == 0) return 0;
        }
    }
    return 1;
}

int select_sudoku_file(char *selected_filename) 
{
    DIR *dir;
    struct dirent *entry;
    struct stat file_stat;
    char file_list[100][256];
    int file_count = 0;
    
    dir = opendir("sudoku");
    if (dir == NULL) {
        printf("Cannot open sudoku directory!\n");
        return 0;
    }
    
    printf("\nAvailable CNF files in sudoku folder:\n");
    printf("No.  Filename                    Size(bytes)\n");
    printf("---  ------------------------  -----------\n");
    
    while ((entry = readdir(dir)) != NULL) {
        if (strstr(entry->d_name, ".cnf") != NULL) {
            char full_path[300];
            sprintf(full_path, "sudoku/%s", entry->d_name);
            
            if (stat(full_path, &file_stat) == 0) {
                printf("%-3d  %-24s  %11ld\n", 
                       file_count + 1, entry->d_name, file_stat.st_size);
                strcpy(file_list[file_count], full_path);
                file_count++;
            }
        }
    }
    closedir(dir);
    
    if (file_count == 0) {
        printf("No CNF files found in sudoku folder!\n");
        return 0;
    }
    
    int choice;
    printf("\nEnter file number (1-%d): ", file_count);
    scanf("%d", &choice);
    
    if (choice < 1 || choice > file_count) {
        printf("Invalid choice!\n");
        return 0;
    }
    
    strcpy(selected_filename, file_list[choice - 1]);
    return 1;
}

void debug_sudoku(int grid[9][9])
{
    printf("\n=== Percent Sudoku Debug Info ===\n");
    int has_error = 0;
    
    // 检查每行
    for (int i = 0; i < 9; i++) {
        int count[10] = {0};
        for (int j = 0; j < 9; j++) {
            if (grid[i][j] >= 1 && grid[i][j] <= 9) {
                count[grid[i][j]]++;
            }
        }
        for (int k = 1; k <= 9; k++) {
            if (count[k] > 1) {
                printf("ERROR: Row %d has duplicate %d (appears %d times)\n", i+1, k, count[k]);
                has_error = 1;
            }
        }
    }
    
    // 检查每列
    for (int j = 0; j < 9; j++) {
        int count[10] = {0};
        for (int i = 0; i < 9; i++) {
            if (grid[i][j] >= 1 && grid[i][j] <= 9) {
                count[grid[i][j]]++;
            }
        }
        for (int k = 1; k <= 9; k++) {
            if (count[k] > 1) {
                printf("ERROR: Column %d has duplicate %d (appears %d times)\n", j+1, k, count[k]);
                has_error = 1;
            }
        }
    }
    
    // 检查3x3盒子
    for (int box = 0; box < 9; box++) {
        int start_row = (box / 3) * 3;
        int start_col = (box % 3) * 3;
        int count[10] = {0};
        
        for (int i = start_row; i < start_row + 3; i++) {
            for (int j = start_col; j < start_col + 3; j++) {
                if (grid[i][j] >= 1 && grid[i][j] <= 9) {
                    count[grid[i][j]]++;
                }
            }
        }
        
        for (int k = 1; k <= 9; k++) {
            if (count[k] > 1) {
                printf("ERROR: Box %d has duplicate %d (appears %d times)\n", box+1, k, count[k]);
                has_error = 1;
            }
        }
    }
    
    // 检查反对角线约束
    int diag_pos[9][2] = {{0, 8}, {1, 7}, {2, 6}, {3, 5}, {4, 4}, {5, 3}, {6, 2}, {7, 1}, {8, 0}};
    int diag_count[10] = {0};
    printf("Anti-diagonal cells: ");
    for (int p = 0; p < 9; p++) {
        int val = grid[diag_pos[p][0]][diag_pos[p][1]];
        printf("(%d,%d)=%d ", diag_pos[p][0]+1, diag_pos[p][1]+1, val);
        if (val >= 1 && val <= 9) {
            diag_count[val]++;
        }
    }
    printf("\n");
    for (int k = 1; k <= 9; k++) {
        if (diag_count[k] > 1) {
            printf("ERROR: Anti-diagonal has duplicate %d (appears %d times)\n", k, diag_count[k]);
            has_error = 1;
        }
    }
    
    // 检查窗口1约束 (1,1)-(3,3)
    int win1_count[10] = {0};
    printf("Window1 (2,2)-(4,4) cells: ");
    for (int i = 1; i <= 3; i++) {
        for (int j = 1; j <= 3; j++) {
            int val = grid[i][j];
            printf("(%d,%d)=%d ", i+1, j+1, val);
            if (val >= 1 && val <= 9) {
                win1_count[val]++;
            }
        }
    }
    printf("\n");
    for (int k = 1; k <= 9; k++) {
        if (win1_count[k] > 1) {
            printf("ERROR: Window1 has duplicate %d (appears %d times)\n", k, win1_count[k]);
            has_error = 1;
        }
    }
    
    // 检查窗口2约束 (5,5)-(7,7)
    int win2_count[10] = {0};
    printf("Window2 (6,6)-(8,8) cells: ");
    for (int i = 5; i <= 7; i++) {
        for (int j = 5; j <= 7; j++) {
            int val = grid[i][j];
            printf("(%d,%d)=%d ", i+1, j+1, val);
            if (val >= 1 && val <= 9) {
                win2_count[val]++;
            }
        }
    }
    printf("\n");
    for (int k = 1; k <= 9; k++) {
        if (win2_count[k] > 1) {
            printf("ERROR: Window2 has duplicate %d (appears %d times)\n", k, win2_count[k]);
            has_error = 1;
        }
    }
    
    // 统计空格子
    int empty_count = 0;
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            if (grid[i][j] == 0) {
                empty_count++;
            }
        }
    }
    printf("Total empty cells: %d\n", empty_count);
    
    if (!has_error) {
        printf("All percent sudoku constraints: PASSED\n");
    } else {
        printf("Percent sudoku constraints: FAILED\n");
    }
    printf("================================\n");
}