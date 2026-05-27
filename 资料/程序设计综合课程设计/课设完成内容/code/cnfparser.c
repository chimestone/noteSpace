#include "cnfparser.h"

cnf *obtain_cnf(const char *filename)
{
    FILE *file = fopen(filename, "r");
    if (file == NULL)
        return NULL;

    cnf *p_cnf = malloc(sizeof(cnf));
    if (p_cnf == NULL) {
        fclose(file);
        return NULL;
    }
    p_cnf->first_clause = NULL;
    p_cnf->arr = NULL;

    char line[1024];
    while (fgets(line, sizeof(line), file))
    {
        if (line[0] == 'c')
            continue;
        if (line[0] == 'p')
        {
            sscanf(line, "p cnf %d %d", &p_cnf->num_literal, &p_cnf->num_clause);
            p_cnf->arr = calloc(p_cnf->num_literal + 1, sizeof(int));
            if (p_cnf->arr == NULL)
            {
                free(p_cnf);
                fclose(file);
                return NULL;
            }
            continue;
        }

        clause *p_clause = malloc(sizeof(clause));
        if (p_clause == NULL)
        {
            free_cnf(p_cnf);
            fclose(file);
            return NULL;
        }
        p_clause->first_literal = NULL;
        p_clause->size = 0;
        p_clause->next = p_cnf->first_clause;
        p_cnf->first_clause = p_clause;

        char *token = strtok(line, " \t\n");
        while (token != NULL && atoi(token) != 0)
        {
            int input = atoi(token);

            literal *p_literal = malloc(sizeof(literal));
            if (p_literal == NULL)
            {
                free_cnf(p_cnf);
                fclose(file);
                return NULL;
            }
            p_literal->name = abs(input);
            p_literal->flag = (input > 0) ? 1 : -1;
            p_literal->next = p_clause->first_literal;
            p_clause->first_literal = p_literal;
            p_clause->size++;

            token = strtok(NULL, " \t\n");
        }
    }

    fclose(file);
    return p_cnf;
}

int evaluate_clause(clause *p_clause, int *arr)
{
    /*
    int none_val = 0;
    */
    literal *p_literal;
    for (p_literal = p_clause->first_literal; p_literal != NULL; p_literal = p_literal->next)
    {
        int val = arr[p_literal->name];
        if ((val == 1 && p_literal->flag == 1) || (val == -1 && p_literal->flag == -1))
        {
            return 1; // 子句为真
        }

        /*
        if(val == 0) {
            none_val = 1;
        }
            */
    }
    return /*none_val ? -1:*/ 0; // 子句为假
}

void free_cnf(cnf *p_cnf)
{
    if (p_cnf == NULL)
        return;

    clause *p_clause = p_cnf->first_clause;
    while (p_clause != NULL)
    {
        literal *p_literal = p_clause->first_literal;
        while (p_literal != NULL)
        {
            literal *p_literal_temp = p_literal -> next;
            free(p_literal);
            p_literal = p_literal_temp;
        }
        clause *p_clause_temp = p_clause->next;
        free(p_clause);
        p_clause = p_clause_temp;
    }
    free(p_cnf -> arr);
    free(p_cnf);
}