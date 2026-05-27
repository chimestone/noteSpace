#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>

// -------------------------- TTTD参数 --------------------------
#define WINDOW_SIZE 50    // 滑动窗口宽度=50
#define TMIN 460          // TTTD最小分块阈值L=460
#define TMAX 2800         // TTTD最大分块阈值T=2800
#define D 540             // TTTD主除数D=540
#define DDASH 270         // TTTD备用除数D'=270
#define BASE 257          // Rabin指纹基数
#define MOD 0xFFFFFFFF   

static uint32_t BASE_POW; 

// 计算BASE^(WINDOW_SIZE-1) mod MOD
static void init_base_pow() {
    BASE_POW = 1;
    for (int i = 0; i < WINDOW_SIZE - 1; i++) {
        BASE_POW = (uint64_t)BASE_POW * BASE % MOD; 
    }
}


// -------------------------- 分块结果数据结构 --------------------------
typedef struct {
    uint64_t start_offset; // 分块起始字节偏移
    uint32_t chunk_size;   // 分块大小（字节）
} Chunk;

typedef struct {
    Chunk* chunks;         // 分块数组
    size_t chunk_count;    // 分块数量
    size_t capacity;       // 数组容量
} ChunkList;


// updateHash函数实现：更新滑动窗口Rabin指纹
static uint32_t update_hash(uint32_t curr_hash, uint8_t out_byte, uint8_t in_byte, bool is_win_full) {
    if (is_win_full) {
        // 移除出窗字节贡献：BASE^(WINDOW_SIZE-1) * out_byte
        uint32_t out_contrib = (uint64_t)out_byte * BASE_POW % MOD;
        curr_hash = (curr_hash + MOD - out_contrib) % MOD; // 防负数
    }
    // 添加入窗字节贡献：hash = (hash*BASE + in_byte) mod MOD
    curr_hash = ((uint64_t)curr_hash * BASE + in_byte) % MOD;
    return curr_hash;
}

// 初始化分块列表
static ChunkList* chunk_list_init() {
    ChunkList* list = (ChunkList*)malloc(sizeof(ChunkList));
    if (!list) { fprintf(stderr, "chunk_list_init: malloc fail (errno=%d)\n", errno); return NULL; }
    list->chunk_count = 0;
    list->capacity = 10; // 初始容量10
    list->chunks = (Chunk*)malloc(list->capacity * sizeof(Chunk));
    if (!list->chunks) { free(list); fprintf(stderr, "chunk_list_init: chunks malloc fail\n"); return NULL; }
    return list;
}

// addBreakpoint函数实现：添加分块边界
static bool add_breakpoint(ChunkList* list, uint64_t start, uint32_t size) {
    if (!list) return false;
    // 容量不足时翻倍扩容
    if (list->chunk_count >= list->capacity) {
        size_t new_cap = list->capacity * 2;
        Chunk* new_chunks = (Chunk*)realloc(list->chunks, new_cap * sizeof(Chunk));
        if (!new_chunks) { fprintf(stderr, "add_breakpoint: realloc fail\n"); return false; }
        list->chunks = new_chunks;
        list->capacity = new_cap;
    }
    list->chunks[list->chunk_count].start_offset = start;
    list->chunks[list->chunk_count].chunk_size = size;
    list->chunk_count++;
    return true;
}

// 释放分块列表内存（防泄漏）
static void chunk_list_free(ChunkList* list) {
    if (list) { free(list->chunks); free(list); }
}

// -------------------------- TTTD分块核心函数 --------------------------
// 对文件执行TTTD分块
ChunkList* tttd_chunking(const char* file_path) {
    // 1. 打开文件（二进制模式）
    FILE* fp = fopen(file_path, "rb");
    if (!fp) { fprintf(stderr, "tttd_chunking: fopen fail (path=%s)\n", file_path); return NULL; }

    // 2. 初始化状态变量
    uint64_t p = 0;                // 当前扫描位置
    uint64_t l = 0;                // 上一分块边界位置
    uint64_t backup_break = 0;     // 备用边界位置
    uint32_t curr_hash = 0;        // 当前窗口哈希
    uint8_t* win_buf = (uint8_t*)malloc(WINDOW_SIZE * sizeof(uint8_t)); // 窗口缓冲区
    size_t win_fill = 0;           // 窗口填充字节数
    ChunkList* chunk_list = chunk_list_init(); // 分块结果

    if (!win_buf || !chunk_list) { free(win_buf); chunk_list_free(chunk_list); fclose(fp); return NULL; }

    // 3. 逐字节扫描文件
    int byte_read;
    while ((byte_read = fgetc(fp)) != EOF) {
        uint8_t in_byte = (uint8_t)byte_read;
        uint8_t out_byte = 0;
        bool is_win_full = false;

        // 3.1 更新窗口缓冲区与哈希
        if (win_fill < WINDOW_SIZE) {
            win_buf[win_fill++] = in_byte;
        } else {
            out_byte = win_buf[0];
            memmove(win_buf, win_buf + 1, WINDOW_SIZE - 1); // 窗口左移
            win_buf[WINDOW_SIZE - 1] = in_byte;
            is_win_full = true;
        }
        curr_hash = update_hash(curr_hash, out_byte, in_byte, is_win_full);

        // 3.2 最小阈值判断：p-l < TMIN时不找边界
        if ((p - l) < TMIN) { p++; continue; }

        // 3.3 备用除数匹配：记录备用边界（hash%DDASH == DDASH-1）
        if ((curr_hash % DDASH) == (DDASH - 1)) { backup_break = p; }

        // 3.4 主除数匹配：设为边界并重置状态（hash%D == D-1）
        if ((curr_hash % D) == (D - 1)) {
            uint32_t size = (uint32_t)(p - l + 1);
            if (!add_breakpoint(chunk_list, l, size)) { free(win_buf); chunk_list_free(chunk_list); fclose(fp); return NULL; }
            backup_break = 0;
            l = p + 1;
            p++;
            continue;
        }

        // 3.5 最大阈值处理：p-l >= TMAX时用备用边界/硬阈值
        if ((p - l) >= TMAX) {
            uint32_t size;
            uint64_t new_l;
            if (backup_break != 0) {
                size = (uint32_t)(backup_break - l + 1);
                new_l = backup_break + 1;
            } else {
                size = (uint32_t)(p - l + 1);
                new_l = p + 1;
            }
            if (!add_breakpoint(chunk_list, l, size)) { free(win_buf); chunk_list_free(chunk_list); fclose(fp); return NULL; }
            backup_break = 0;
            l = new_l;
        }

        p++;
    }

    // 4. 处理文件末尾剩余字节
    if (l <= p - 1) {
        uint32_t last_size = (uint32_t)((p - 1) - l + 1);
        add_breakpoint(chunk_list, l, last_size);
    }

    // 5. 释放资源并返回
    free(win_buf);
    fclose(fp);
    return chunk_list;
}


// 检查分块是否符合约束（大小、完整性）
static bool check_chunk_constraints(const ChunkList* list, uint64_t file_size) {
    if (!list || list->chunk_count == 0) { fprintf(stderr, "check: invalid chunk list\n"); return false; }

    uint64_t total_size = 0;
    bool all_valid = true;

    for (size_t i = 0; i < list->chunk_count; i++) {
        const Chunk* c = &list->chunks[i];
        total_size += c->chunk_size;

        // 约束1：分块大小≤TMAX
        if (c->chunk_size > TMAX) {
            fprintf(stderr, "check: Chunk%d(size=%u) > TMAX=%d\n", (int)i+1, c->chunk_size, TMAX);
            all_valid = false;
        }

        // 约束2：除最后一块外，大小≥TMIN
        if (i != list->chunk_count - 1 && c->chunk_size < TMIN) {
            fprintf(stderr, "check: Chunk%d(size=%u) < TMIN=%d\n", (int)i+1, c->chunk_size, TMIN);
            all_valid = false;
        }

        // 约束3：分块无越界
        if (c->start_offset + c->chunk_size > file_size) {
            fprintf(stderr, "check: Chunk%d exceeds file size\n", (int)i+1);
            all_valid = false;
        }
    }

    // 约束4：分块总大小=文件大小
    if (total_size != file_size) {
        fprintf(stderr, "check: total chunk size=%llu != file size=%llu\n", (unsigned long long)total_size, (unsigned long long)file_size);
        all_valid = false;
    }

    return all_valid;
}

// 测试TTTD分块
void test_tttd(const char* file_path) {
    printf("=== TTTD Test (A_Framework_for_Analyzing_and_Improving_Content-Ba.pdf) ===\n");
    printf("Test File: %s\n", file_path);

    // 1. 获取文件大小
    FILE* fp = fopen(file_path, "rb");
    if (!fp) { fprintf(stderr, "test: fopen fail\n"); return; }
    fseek(fp, 0, SEEK_END);
    uint64_t file_size = ftell(fp);
    fclose(fp);
    printf("File Size: %llu bytes\n", (unsigned long long)file_size);

    // 2. 执行TTTD分块
    ChunkList* chunks = tttd_chunking(file_path);
    if (!chunks) { fprintf(stderr, "test: TTTD chunking fail\n"); return; }
    printf("Generated Chunks: %zu\n", chunks->chunk_count);
    printf("TTTD Params : WINDOW=%d, TMIN=%d, TMAX=%d, D=%d, DDASH=%d\n",
           WINDOW_SIZE, TMIN, TMAX, D, DDASH);

    // 3. 验证约束
    printf("\n--- Appendix B Constraint Check ---\n");
    bool valid = check_chunk_constraints(chunks, file_size);
    printf("All Constraints Met: %s\n", valid ? "YES" : "NO");

    // 4. 打印前10个分块
    printf("\n--- First 10 Chunks ---\n");
    printf("%-8s %-18s %-12s\n", "ID", "Start Offset", "Size");
    size_t print_cnt = chunks->chunk_count > 10 ? 10 : chunks->chunk_count;
    for (size_t i = 0; i < print_cnt; i++) {
        const Chunk* c = &chunks->chunks[i];
        printf("%-8zu %-18llu %-12u\n", i+1, (unsigned long long)c->start_offset, c->chunk_size);
    }
    if (chunks->chunk_count > 10) {
        printf("... (total %zu chunks, %zu not shown)\n", chunks->chunk_count, chunks->chunk_count - 10);
    }

    chunk_list_free(chunks);
    printf("\n=== Test Done ===\n");
}


int main(int argc, char* argv[]) {
    init_base_pow();
  
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <test_file_path>\n", argv[0]);
        fprintf(stderr, "Example: %s sample.txt\n", argv[0]);
        return EXIT_FAILURE;
    }

    // 执行测试
    test_tttd(argv[1]);
    return EXIT_SUCCESS;
}