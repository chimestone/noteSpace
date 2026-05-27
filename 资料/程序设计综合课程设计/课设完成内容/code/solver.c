#include "solver.h"

int find_unit_clause(cnf *p_cnf)
{
    clause *p_clause = p_cnf->first_clause;
    for (; p_clause != NULL; p_clause = p_clause->next)
    {
        if (evaluate_clause(p_clause, p_cnf->arr) == 1)
            continue;

        int flag = 0;
        int unit_clause = 0;
        literal *p_literal = p_clause->first_literal;

        for (; p_literal != NULL; p_literal = p_literal->next)
        {
            if (p_cnf->arr[p_literal->name] == 0)
            {
                flag++;
                unit_clause = p_literal->name * p_literal->flag;
            }
        }

        if (flag == 0)
            return -1;
        if (flag == 1)
            return unit_clause;
    }
    return 0;
}

int dpll_recursion(cnf *p_cnf)
{
    int unit;

    while ((unit = find_unit_clause(p_cnf)) != 0)
    {
        if (unit == -1)
            return 0;
        p_cnf->arr[abs(unit)] = unit > 0 ? 1 : -1;
    }

    clause *p_clause = p_cnf->first_clause;
    for (; p_clause != NULL; p_clause = p_clause->next)
    {
        if (evaluate_clause(p_clause, p_cnf->arr) == 0)
        {
            int all_signed = 1;
            literal *p_literal = p_clause->first_literal;
            for (; p_literal != NULL; p_literal = p_literal->next)
            {
                if (p_cnf->arr[p_literal->name] == 0)
                {
                    all_signed = 0;
                    break;
                }
            }
            if (all_signed == 1)
                return 0;
        }
    }

    int var = 0;
    for (int i = 1; i <= p_cnf->num_literal; i++)
    {
        if (p_cnf->arr[i] == 0)
        {
            var = i;
            break;
        }
    }
    if (var == 0)
        return 1;

    int *backup = malloc(sizeof(int) * (p_cnf->num_literal + 1));
    if (backup == NULL)
        return 0;
    memcpy(backup, p_cnf->arr, sizeof(int) * (p_cnf->num_literal + 1));

    p_cnf->arr[var] = 1;
    if (dpll_recursion(p_cnf) == 1)
    {
        free(backup);
        return 1;
    }

    memcpy(p_cnf->arr, backup, sizeof(int) * (p_cnf->num_literal + 1));

    p_cnf->arr[var] = -1;
    int result = dpll_recursion(p_cnf);

    if (result == 0)
        memcpy(p_cnf->arr, backup, sizeof(int) * (p_cnf->num_literal + 1));
    free(backup);
    return result;
}

int dpll_solve(cnf *p_cnf)
{
    return dpll_recursion(p_cnf);
}

int q_find_fun(cnf *p_cnf)
{
    int *freq = calloc(p_cnf->num_literal + 1, sizeof(int));
    if (freq == NULL) return 0;
    
    clause *p_clause = p_cnf->first_clause;
    for (; p_clause != NULL; p_clause = p_clause->next)
    {
        if (evaluate_clause(p_clause, p_cnf->arr) == 1)
            continue;
            
        literal *p_literal = p_clause->first_literal;
        for (; p_literal != NULL; p_literal = p_literal->next)
        {
            if (p_cnf->arr[p_literal->name] == 0)
                freq[p_literal->name]++;
        }
    }
    
    int max_freq = 0, chosen = 0;
    for (int i = 1; i <= p_cnf->num_literal; i++)
    {
        if (p_cnf->arr[i] == 0 && freq[i] > max_freq)
        {
            max_freq = freq[i];
            chosen = i;
        }
    }
    
    free(freq);
    return chosen;
}

int q_dpll_recursion(cnf *p_cnf)
{
    int unit;
    while ((unit = find_unit_clause(p_cnf)) != 0)
    {
        if (unit == -1) return 0;
        p_cnf->arr[abs(unit)] = unit > 0 ? 1 : -1;
    }

    clause *p_clause = p_cnf->first_clause;
    for (; p_clause != NULL; p_clause = p_clause->next)
    {
        if (evaluate_clause(p_clause, p_cnf->arr) == 0)
        {
            int all_signed = 1;
            literal *p_literal = p_clause->first_literal;
            for (; p_literal != NULL; p_literal = p_literal->next)
            {
                if (p_cnf->arr[p_literal->name] == 0)
                {
                    all_signed = 0;
                    break;
                }
            }
            if (all_signed == 1) return 0;
        }
    }

    int var = q_find_fun(p_cnf);
    if (var == 0) return 1;

    int *backup = malloc(sizeof(int) * (p_cnf->num_literal + 1));
    if (backup == NULL) return 0;
    memcpy(backup, p_cnf->arr, sizeof(int) * (p_cnf->num_literal + 1));

    p_cnf->arr[var] = 1;
    if (q_dpll_recursion(p_cnf) == 1)
    {
        free(backup);
        return 1;
    }

    memcpy(p_cnf->arr, backup, sizeof(int) * (p_cnf->num_literal + 1));
    p_cnf->arr[var] = -1;
    int result = q_dpll_recursion(p_cnf);

    if (result == 0)
        memcpy(p_cnf->arr, backup, sizeof(int) * (p_cnf->num_literal + 1));
    free(backup);
    return result;
}

int q_dpll_solve(cnf *p_cnf)
{
    return q_dpll_recursion(p_cnf);
}

void save_result(int result, clock_t time, cnf *p_cnf, const char *filename)
{
    char save_filename[256];
    
    // 提取文件名（不包含路径）
    const char *basename = strrchr(filename, '\\');
    if (basename == NULL)
        basename = strrchr(filename, '/');
    if (basename == NULL)
        basename = filename;
    else
        basename++; // 跳过路径分隔符
    
    // 构建保存到output文件夹的路径
    strcpy(save_filename, "output/");
    strcat(save_filename, basename);
    
    char *dot = strrchr(save_filename, '.');
    if (dot != NULL)
        strcpy(dot, ".res");
    else
        strcat(save_filename, ".res");

    FILE *file = fopen(save_filename, "w");
    if (file == NULL)
    {
        printf("Failed to open file %s!\n", save_filename);
        return;
    }

    fprintf(file, "s %d\n", result);
    if (result == 1)
    {
        fprintf(file, "v ");
        for (int i = 1; i <= p_cnf->num_literal; i++)
        {
            fprintf(file, "%d ", p_cnf->arr[i] == 1 ? i : -i);
        }
        fprintf(file, "\n");
    }
    fprintf(file, "t %ld\n", time);
    fclose(file);
}