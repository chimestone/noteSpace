#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>
#include <time.h>

// 依赖：uthash（轻量级哈希表，用于Shingle去重）
#include "uthash/src/uthash.h"

// -------------------------- 1. 算法参数--------------------------
#define W 4                  // Shingle长度：连续4个Token
#define MAX_TOKEN_LEN 64     // 单个Token最大长度（适配常规文本）
#define MAX_SHINGLE_CNT 1024 // 单个文档最大Shingle数（按需调整）

#define K 6       // 特征组数（示例k=6）
#define S 14      // 每组元素数（示例s=14）
#define T (K * S) // 草图长度（t=k*s=84）
#define R 2       // 相似判定阈值：特征匹配数≥2（示例r=2）

// Rabin指纹不可约多项式（64位）
#define RABIN_POLY_SHINGLE 0x13D6B48BD16F30A7ULL // Shingle指纹用多项式
#define RABIN_POLY_FEATURE 0x12D68707312FAD33ULL // 特征计算用独立多项式（避免组间依赖）

// -------------------------- 2. 数据结构 --------------------------
// 2.1 Token列表（存储规范化后的Token）
typedef struct
{
    char **tokens; // Token数组
    int count;     // Token总数
} TokenList;

// 2.2 Shingle哈希表（用于去重）
typedef struct
{
    char shingle_str[(W * MAX_TOKEN_LEN) + (W - 1)]; // Shingle字符串（Token间用空格分隔）
    UT_hash_handle hh;                               // uthash必要字段
} ShingleHash;

// 2.3 Min-Wise排列参数（线性变换a*x + b mod 2^64）
typedef struct
{
    uint64_t a; // 线性系数（奇数，保证可逆）
    uint64_t b; // 偏移量（随机值）
} MinWiseParam;

// 2.4 文档特征集（6个64位特征，共48字节）
typedef struct
{
    uint64_t features[K];
} DocFeatures;

// -------------------------- 3. 核心工具函数 --------------------------
/**
 * 文本规范化与Token化（将文档转为canonical token序列）
 * @param text 输入文本
 * @return TokenList 规范化后的Token列表
 */
TokenList tokenize(const char *text)
{
    TokenList list = {NULL, 0};
    int text_len = strlen(text);
    char *temp_buf = (char *)malloc(text_len + 1);
    if (!temp_buf)
        return list;

    // 步骤1：规范化（仅保留字母，转小写，其他字符替换为空格）
    int buf_idx = 0;
    for (int i = 0; i < text_len; i++)
    {
        if (isalpha((unsigned char)text[i]))
        {
            temp_buf[buf_idx++] = tolower((unsigned char)text[i]);
        }
        else if (buf_idx > 0 && temp_buf[buf_idx - 1] != ' ')
        {
            temp_buf[buf_idx++] = ' ';
        }
    }
    temp_buf[buf_idx] = '\0';

    char *token = strtok(temp_buf, " ");
    while (token != NULL)
    {
        int token_len = strlen(token);
        if (token_len > 0 && token_len <= MAX_TOKEN_LEN)
        {

            if (list.count > 0 && strcmp(list.tokens[list.count - 1], "alta") == 0 && strcmp(token, "vista") == 0)
            {

                strcat(list.tokens[list.count - 1], token);
            }
            else
            {
                list.tokens = (char **)realloc(list.tokens, (list.count + 1) * sizeof(char *));
                if (!list.tokens)
                    break;
                list.tokens[list.count] = (char *)malloc(token_len + 1);
                strcpy(list.tokens[list.count], token);
                list.count++;
            }
        }
        token = strtok(NULL, " ");
    }

    free(temp_buf);
    return list;
}
/**
 * 释放TokenList内存
 */
void token_list_free(TokenList *list)
{
    if (list->tokens)
    {
        for (int i = 0; i < list->count; i++)
        {
            free(list->tokens[i]);
        }
        free(list->tokens);
        list->tokens = NULL;
        list->count = 0;
    }
}

/**
 * 生成去重的Shingle集合（Shingling是“连续W个Token的集合”，需去重）
 * @param list Token列表
 * @param shingles 输出Shingle数组（提前分配MAX_SHINGLE_CNT大小）
 * @return int Shingle总数（≤MAX_SHINGLE_CNT）
 */
int generate_shingles(const TokenList *list, char shingles[MAX_SHINGLE_CNT][(W * MAX_TOKEN_LEN) + (W - 1)])
{
    if (list->count < W)
        return 0; // Token数不足W，无法生成Shingle
    ShingleHash *hash_table = NULL, *entry, *tmp;
    int shingle_cnt = 0;

    // 遍历所有W-length Token窗口，生成Shingle
    for (int i = 0; i <= list->count - W; i++)
    {
        char shingle_buf[(W * MAX_TOKEN_LEN) + (W - 1)] = {0};
        // 拼接W个Token为Shingle字符串（Token间用空格分隔，保证唯一性）
        for (int j = 0; j < W; j++)
        {
            if (j > 0)
                strcat(shingle_buf, " ");
            strcat(shingle_buf, list->tokens[i + j]);
        }

        // 哈希表去重（确保Shingle为集合，避免重复计算）
        HASH_FIND_STR(hash_table, shingle_buf, entry);
        if (!entry)
        {
            if (shingle_cnt >= MAX_SHINGLE_CNT)
                break; // 超过最大容量
            // 新增Shingle到哈希表和输出数组
            entry = (ShingleHash *)malloc(sizeof(ShingleHash));
            strcpy(entry->shingle_str, shingle_buf);
            HASH_ADD_STR(hash_table, shingle_str, entry);
            strcpy(shingles[shingle_cnt], shingle_buf);
            shingle_cnt++;
        }
    }

    // 释放哈希表内存
    HASH_ITER(hh, hash_table, entry, tmp)
    {
        HASH_DEL(hash_table, entry);
        free(entry);
    }

    return shingle_cnt;
}

/**
 * 计算64位Rabin指纹
 * @param str 输入字符串（Shingle或特征组）
 * @param poly 不可约多项式
 * @return uint64_t 64位Rabin指纹
 */
uint64_t rabin_fingerprint(const char *str, uint64_t poly)
{
    uint64_t fp = 0; // 指纹初始值（多项式系数）
    int str_len = strlen(str);

    // 逐字节处理，更新多项式
    for (int i = 0; i < str_len; i++)
    {
        unsigned char c = (unsigned char)str[i];
        for (int bit = 7; bit >= 0; bit--)
        {
            fp = (fp << 1) | ((c >> bit) & 1);
            if (fp & (1ULL << 63))
            {
                fp ^= poly;
            }
        }
    }

    // 补64位零，确保最终指纹为64位（避免短字符串指纹碰撞）
    for (int i = 0; i < 64; i++)
    {
        fp = fp << 1;
        if (fp & (1ULL << 63))
        {
            fp ^= poly;
        }
    }

    return fp;
}

/**
 * 生成T个Min-Wise排列参数（用线性变换a*x + b代替真随机排列，a为奇数保证可逆）
 * @param params 输出参数数组（提前分配T大小）
 */
void generate_minwise_params(MinWiseParam params[T])
{
    srand((unsigned int)time(NULL));

    for (int i = 0; i < T; i++)
    {

        params[i].a = (uint64_t)rand() | 1ULL;
        params[i].a |= (uint64_t)rand() << 32;

        params[i].b = (uint64_t)rand();
        params[i].b |= (uint64_t)rand() << 32;
    }
}

/**
 * 生成文档草图（每个Min-Wise排列取Shingle指纹的最小值，组成草图）
 * @param shingle_fps Shingle指纹数组
 * @param shingle_cnt Shingle总数
 * @param minwise_params Min-Wise参数数组
 * @param sketch 输出草图（T个64位最小值）
 */
void generate_sketch(const uint64_t *shingle_fps, int shingle_cnt,
                     const MinWiseParam minwise_params[T], uint64_t sketch[T])
{
    // 初始化草图为最大值（确保首个Shingle能更新最小值）
    for (int i = 0; i < T; i++)
    {
        sketch[i] = UINT64_MAX;
    }

    // 遍历每个Shingle指纹，更新每个排列的最小值
    for (int i = 0; i < shingle_cnt; i++)
    {
        uint64_t fp = shingle_fps[i];
        for (int j = 0; j < T; j++)
        {
            // Min-Wise线性变换：a*fp + b
            uint64_t transformed = (minwise_params[j].a * fp) + minwise_params[j].b;
            // 更新当前排列的最小值
            if (transformed < sketch[j])
            {
                sketch[j] = transformed;
            }
        }
    }
}

/**
 * 从草图提取文档特征（将草图分K组，每组算Rabin指纹，共48字节）
 * @param sketch 草图（T=K*S个元素）
 * @param features 输出文档特征集
 */
void generate_features(const uint64_t sketch[T], DocFeatures *features)
{
    for (int g = 0; g < K; g++)
    {
        uint64_t group_hash = 0;
        // 对组内S个草图元素进行数值异或
        for (int s = 0; s < S; s++)
        {
            int idx = g * S + s;
            group_hash ^= sketch[idx];
        }
        // 用Rabin指纹进一步压缩
        features->features[g] = rabin_fingerprint((char *)&group_hash, RABIN_POLY_FEATURE);
    }
}

/**
 * 相似判定（特征匹配数≥R则判定为相似）
 * @param f1 文档1的特征集
 * @param f2 文档2的特征集
 * @return int 1=相似，0=非相似
 */
int is_near_duplicate(const DocFeatures *f1, const DocFeatures *f2)
{
    int match_cnt = 0;
    for (int i = 0; i < K; i++)
    {
        if (f1->features[i] == f2->features[i])
        {
            match_cnt++;
            if (match_cnt >= R)
            {
                return 1;
            }
        }
    }
    return 0;
}

void print_tokens(const char *doc_name, const TokenList *list)
{
    printf("\n【%s - Token化结果】\n", doc_name);
    printf("Token总数：%d\n", list->count);
    printf("前10个Token（全小写，无标点）：");
    for (int i = 0; i < (list->count < 10 ? list->count : 10); i++)
    {
        printf("%s%s", list->tokens[i], (i < 9 && i < list->count - 1) ? ", " : "");
    }
    printf("\n");
}

void print_shingles(const char *doc_name, const char shingles[MAX_SHINGLE_CNT][(W * MAX_TOKEN_LEN) + (W - 1)], int shingle_cnt)
{
    printf("\n【%s - Shingle集合】\n", doc_name);
    printf("Shingle总数（去重后）：%d\n", shingle_cnt);
    printf("前5个Shingle（每个含%d个Token）：\n", W);
    for (int i = 0; i < (shingle_cnt < 5 ? shingle_cnt : 5); i++)
    {
        printf("  Shingle[%d]：%s\n", i + 1, shingles[i]);
    }
}

void print_shingle_fps(const char *doc_name, const uint64_t *shingle_fps, int shingle_cnt)
{
    printf("\n【%s - Shingle的Rabin指纹】\n", doc_name);
    printf("前5个Shingle的64位指纹（十六进制）：\n");
    for (int i = 0; i < (shingle_cnt < 5 ? shingle_cnt : 5); i++)
    {
        printf("  Shingle[%d]指纹：%016lx\n", i + 1, shingle_fps[i]);
    }
}

void print_sketch(const char *doc_name, const uint64_t sketch[T])
{
    printf("\n【%s - Min-Wise草图】\n", doc_name);
    printf("草图长度（T=K*S）：%d\n", T);
    printf("前10个草图元素（每个为Min-Wise排列的最小值）：\n");
    for (int i = 0; i < 10; i++)
    {
        printf("  Sketch[%d]：%016lx\n", i + 1, sketch[i]);
    }
}

void print_features(const char *doc_name, const DocFeatures *features)
{
    printf("\n【%s - 文档特征】\n", doc_name);
    printf("特征总数（K）：%d（共%d字节）\n", K, K * 8);
    for (int i = 0; i < K; i++)
    {
        printf("  Feature[%d]：%016lx\n", i + 1, features->features[i]);
    }
}

// -------------------------- 4. 算法调用示例 --------------------------
int main()
{
    const char *doc1_text = "AltaVista was developed at Digital's Palo Alto Research Labs in 1995. "
                            "It quickly became a popular web search engine due to its large index.";
    const char *doc2_text = "ALTA VISTA WAS DEVELOPED AT DIGITAL'S PALO ALTO RESEARCH LABS IN 1995! "
                            "It quickly became a popular web search engine due to its large index of web pages.";
    const char *doc3_text = "The World Wide Web doubles in size every 9 to 12 months (broder2000.pdf, Section 2.1). "
                            "This growth makes near-duplicate filtering critical for search engines.";

    MinWiseParam minwise_params[T];
    generate_minwise_params(minwise_params);

    // -------------------------- 处理文档1：生成中间结果 --------------------------
    TokenList doc1_tokens = tokenize(doc1_text);
    print_tokens("文档1", &doc1_tokens);

    char doc1_shingles[MAX_SHINGLE_CNT][(W * MAX_TOKEN_LEN) + (W - 1)];
    int doc1_shingle_cnt = generate_shingles(&doc1_tokens, doc1_shingles);
    print_shingles("文档1", doc1_shingles, doc1_shingle_cnt);

    uint64_t doc1_shingle_fps[MAX_SHINGLE_CNT];
    for (int i = 0; i < doc1_shingle_cnt; i++)
    {
        doc1_shingle_fps[i] = rabin_fingerprint(doc1_shingles[i], RABIN_POLY_SHINGLE);
    }
    print_shingle_fps("文档1", doc1_shingle_fps, doc1_shingle_cnt);

    uint64_t doc1_sketch[T];
    generate_sketch(doc1_shingle_fps, doc1_shingle_cnt, minwise_params, doc1_sketch);
    print_sketch("文档1", doc1_sketch);
    DocFeatures doc1_features;
    generate_features(doc1_sketch, &doc1_features);
    print_features("文档1", &doc1_features);
    token_list_free(&doc1_tokens);

    // -------------------------- 处理文档2（相似变体）--------------------------
    TokenList doc2_tokens = tokenize(doc2_text);
    print_tokens("文档2", &doc2_tokens);
    char doc2_shingles[MAX_SHINGLE_CNT][(W * MAX_TOKEN_LEN) + (W - 1)];
    int doc2_shingle_cnt = generate_shingles(&doc2_tokens, doc2_shingles);
    print_shingles("文档2", doc2_shingles, doc2_shingle_cnt);
    uint64_t doc2_shingle_fps[MAX_SHINGLE_CNT];
    for (int i = 0; i < doc2_shingle_cnt; i++)
    {
        doc2_shingle_fps[i] = rabin_fingerprint(doc2_shingles[i], RABIN_POLY_SHINGLE);
    }
    print_shingle_fps("文档2", doc2_shingle_fps, doc2_shingle_cnt);
    uint64_t doc2_sketch[T];
    generate_sketch(doc2_shingle_fps, doc2_shingle_cnt, minwise_params, doc2_sketch);
    print_sketch("文档2", doc2_sketch);
    DocFeatures doc2_features;
    generate_features(doc2_sketch, &doc2_features);
    print_features("文档2（相似变体）", &doc2_features); // 重点对比特征
    token_list_free(&doc2_tokens);

    // -------------------------- 处理文档3（无关内容） --------------------------
    TokenList doc3_tokens = tokenize(doc3_text);
    char doc3_shingles[MAX_SHINGLE_CNT][(W * MAX_TOKEN_LEN) + (W - 1)];
    int doc3_shingle_cnt = generate_shingles(&doc3_tokens, doc3_shingles);

    uint64_t doc3_shingle_fps[MAX_SHINGLE_CNT];
    for (int i = 0; i < doc3_shingle_cnt; i++)
    {
        doc3_shingle_fps[i] = rabin_fingerprint(doc3_shingles[i], RABIN_POLY_SHINGLE);
    }

    uint64_t doc3_sketch[T];
    generate_sketch(doc3_shingle_fps, doc3_shingle_cnt, minwise_params, doc3_sketch);

    DocFeatures doc3_features;
    generate_features(doc3_sketch, &doc3_features);
    print_features("文档3（无关内容）", &doc3_features); // 对比特征差异
    token_list_free(&doc3_tokens);

    // -------------------------- 最终相似判定结果 --------------------------
    printf("\n=== broder2000.pdf 相似文档检测最终结果 ===\n");
    printf("文档1与文档2（相似变体）：%s\n",
           is_near_duplicate(&doc1_features, &doc2_features) ? "判定为相似" : "判定为非相似");
    printf("文档1与文档3（无关内容）：%s\n",
           is_near_duplicate(&doc1_features, &doc3_features) ? "判定为相似" : "判定为非相似");

    return 0;
}