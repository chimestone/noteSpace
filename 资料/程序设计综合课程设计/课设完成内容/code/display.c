#include "display.h"
#include "sudoku.h"
#ifdef _WIN32
#include <windows.h>
#endif

void print_menu(void)
{
    printf("\n=== Main Menu ===\n");
    printf("1. CNF file solving\n");
    printf("2. Percent Sudoku game\n");
    printf("3. Exit\n");
    printf("Enter your choice: ");
}

void print_sec_menu(void)
{
    printf("\n=== CNF Solver Menu ===\n");
    printf("1. Use DPLL strategy\n");
    printf("2. Use Q-DPLL strategy and display the optimization rate\n");
    printf("3. Init-display\n");
    printf("4. Back to Main Menu\n");
    printf("Enter your choice: ");
}

void print_result(int result, clock_t time, cnf *p_cnf)
{
    printf("The CNF file has %d literals and %d clauses.\n", p_cnf->num_literal, p_cnf->num_clause);
    printf("Result: %s\n", result ? "SAT" : "UNSAT");
    if (time == 0)
    {
        printf("Time taken: < 1 ms\n");
    }
    else
    {
        printf("Time taken: %ld ms\n", time);
    }
}

void print_clause(cnf *p_cnf)
{
    int clauses = p_cnf->num_clause;
    clause *p_clause = p_cnf->first_clause;
    for (int i = 0; i < clauses; i++)
    {
        int literals = p_clause->size;
        literal *p_literal = p_clause->first_literal;
        printf("Clause %d: ", i + 1);
        for (int j = 0; j < literals; j++)
        {
            if (p_literal->flag == 1)
            {
                printf("%d ", p_literal->name);
            }
            else
            {
                printf("-%d ", p_literal->name);
            }
            p_literal = p_literal->next;
        }
        printf("0\n");
        p_clause = p_clause->next;
    }
}

void print_improve(clock_t whole_time, clock_t q_whole_time, int result)
{
    if (result == 1)
    {
        if (whole_time == 0 && q_whole_time == 0)
        {
            printf("The optimization rate: Both algorithms completed in < 1 ms\n");
        }
        else if (whole_time == 0)
        {
            printf("The optimization rate: DPLL < 1 ms, Q-DPLL %ld ms\n", q_whole_time);
        }
        else
        {
            double rate = (1 - (double)q_whole_time / whole_time) * 100;
            printf("The optimization rate is: %.2f%%\n", rate);
        }
    }
}

int main(void)
{
    int choice;
    char filename[256];

    while (1)
    {
        print_menu();
        scanf("%d", &choice);
        switch (choice)
        {
        case 1:
            cnf *get_cnf;
            // CNF解析模块
            if (!select_cnf_file(filename))
            {
                printf("No file selected.\n");
                break;
            }
            while (1)
            {
                print_sec_menu();
                int sec_choice;
                scanf("%d", &sec_choice);

                if (sec_choice == 4)
                {
                    break;
                }

                get_cnf = obtain_cnf(filename);

                if (get_cnf == NULL)
                {
                    printf("Failed to obtain CNF formula information from %s\n", filename);
                    break;
                }

                if (sec_choice == 5)
                {
                    clock_t start = clock();
                    int result = q_dpll_solve(get_cnf);
                    clock_t end = clock();
                    clock_t time = ((double)(end - start) * 1000.0) / CLOCKS_PER_SEC;
                    print_result(result, time, get_cnf);
                    save_result(result, time, get_cnf, filename);
                    free_cnf(get_cnf);
                    break;
                }
                if (sec_choice == 3)
                {
                    print_clause(get_cnf);
                    continue;
                }
                clock_t start = clock();
                int result = dpll_solve(get_cnf);
                clock_t end = clock();
                double time_ms = ((double)(end - start) * 1000.0) / CLOCKS_PER_SEC;
                clock_t whole_time = (clock_t)time_ms;

                print_result(result, whole_time, get_cnf);
                save_result(result, whole_time, get_cnf, filename);

                if (sec_choice == 2)
                {
                    // 重置CNF状态
                    for (int i = 1; i <= get_cnf->num_literal; i++)
                        get_cnf->arr[i] = 0;

                    clock_t q_start = clock();
                    int q_result = q_dpll_solve(get_cnf);
                    clock_t q_end = clock();
                    double q_time_ms = ((double)(q_end - q_start) * 1000.0) / CLOCKS_PER_SEC;
                    clock_t q_whole_time = (clock_t)q_time_ms;

                    print_result(q_result, q_whole_time, get_cnf);
                    char q_filename[256];
                    sprintf(q_filename, "q_%s", filename);
                    save_result(q_result, q_whole_time, get_cnf, q_filename);

                    print_improve(whole_time, q_whole_time, result);
                }
                free_cnf(get_cnf);
            }
            free_cnf(get_cnf);
            break;
        case 2:
            // Percent Sudoku module
            percent_sudoku_menu();
            break;
        case 3:
            printf("Program ended, goodbye!\n");
            getchar();
            return 0;
        default:
            // Invalid input prompt
            printf("Invalid input!\nPlease enter again!\n");
        }
    }

    return 0;
}

int select_cnf_file(char *selected_filename)
{
    DIR *dir;
    struct dirent *entry;
    struct stat file_stat;
    char file_list[100][256];
    int file_count = 0;

    dir = opendir("input");
    if (dir == NULL)
    {
        printf("Cannot open input directory!\n");
        return 0;
    }

    printf("\nAvailable CNF files in input folder:\n");
    printf("No.  Filename                    Size(bytes)\n");
    printf("---  ------------------------  -----------\n");

    while ((entry = readdir(dir)) != NULL)
    {
        if (strstr(entry->d_name, ".cnf") != NULL)
        {
            char full_path[300];
            sprintf(full_path, "input/%s", entry->d_name);

            if (stat(full_path, &file_stat) == 0)
            {
                printf("%-3d  %-24s  %11ld\n",
                       file_count + 1, entry->d_name, file_stat.st_size);
                strcpy(file_list[file_count], full_path);
                file_count++;
            }
        }
    }
    closedir(dir);

    if (file_count == 0)
    {
        printf("No CNF files found in input folder!\n");
        return 0;
    }

    int choice;
    printf("\nEnter file number (1-%d): ", file_count);
    scanf("%d", &choice);

    if (choice < 1 || choice > file_count)
    {
        printf("Invalid choice!\n");
        return 0;
    }

    strcpy(selected_filename, file_list[choice - 1]);
    return 1;
}