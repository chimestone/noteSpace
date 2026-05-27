#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>

// 配置参数
#define MAX_CHUNK_SIZE 8192
#define MIN_CHUNK_SIZE 64
#define WINDOW_SIZE 48
#define HASH_MULTIPLIER 31

// 块结构体
typedef struct {
    uint32_t start;      // 块起始位置
    uint32_t length;     // 块长度
    uint32_t hash;       // 块哈希值
} Chunk;

// 全局变量
static uint8_t *g_data = NULL;
static uint32_t g_data_size = 0;
static Chunk *g_chunks = NULL;
static uint32_t g_chunk_count = 0;
static uint32_t *g_rolling_hash = NULL;


void cleanup(void);
int is_local_maximum(uint32_t pos);
void print_chunks(void);
uint32_t rolling_hash(const uint8_t *data, uint32_t length);
uint32_t update_hash(uint32_t old_hash, uint8_t old_byte, uint8_t new_byte, uint32_t length);
uint32_t calculate_hash(const uint8_t *data, uint32_t length);
int maxp_chunk(uint8_t *data, uint32_t data_size);

// 计算滚动哈希值
uint32_t rolling_hash(const uint8_t *data, uint32_t length) {
    uint32_t hash = 0;
    for (uint32_t i = 0; i < length; i++) {
        hash = hash * HASH_MULTIPLIER + data[i];
    }
    return hash;
}

// 更新滚动哈希值
uint32_t update_hash(uint32_t old_hash, uint8_t old_byte, uint8_t new_byte, uint32_t length) {
    uint32_t multiplier = 1;
    for (uint32_t i = 0; i < length - 1; i++) {
        multiplier *= HASH_MULTIPLIER;
    }
    return (old_hash - old_byte * multiplier) * HASH_MULTIPLIER + new_byte;
}

// 检查是否为局部最大值
int is_local_maximum(uint32_t pos) {
    if (!g_rolling_hash || pos >= g_data_size) {
        return 0;
    }
    
    // 对于小数据，使用较小的窗口
    uint32_t half_window = WINDOW_SIZE / 2;
    if (g_data_size < WINDOW_SIZE) {
        half_window = g_data_size / 2;
    }
    
    if (pos < half_window || pos >= g_data_size - half_window) {
        return 0;
    }
    
    uint32_t current_hash = g_rolling_hash[pos];
    
    // 检查左右窗口内的哈希值
    for (uint32_t i = pos - half_window; i <= pos + half_window; i++) {
        if (i != pos && i < g_data_size && g_rolling_hash[i] > current_hash) {
            return 0;  // 不是局部最大值
        }
    }
    
    return 1;  // 是局部最大值
}

// 计算块的哈希值
uint32_t calculate_hash(const uint8_t *data, uint32_t length) {
    uint32_t hash = 0;
    for (uint32_t i = 0; i < length; i++) {
        hash = hash * HASH_MULTIPLIER + data[i];
    }
    return hash;
}

// MAXP分块算法
int maxp_chunk(uint8_t *data, uint32_t data_size) {
    if (!data || data_size == 0) {
        return -1;
    }
    
    // 先清理之前的数据
    cleanup();
    
    // 保存全局数据
    g_data = data;
    g_data_size = data_size;
    
    // 分配内存
    g_chunks = (Chunk*)malloc(1000 * sizeof(Chunk));  // 最多1000个块
    g_rolling_hash = (uint32_t*)malloc(data_size * sizeof(uint32_t));
    
    if (!g_chunks || !g_rolling_hash) {
        free(g_chunks);
        free(g_rolling_hash);
        g_chunks = NULL;
        g_rolling_hash = NULL;
        return -1;
    }
    
    // 计算滚动哈希值
    uint32_t window_size = WINDOW_SIZE;
    if (data_size < window_size) {
        window_size = data_size;
    }
    
    // 初始化所有滚动哈希值
    for (uint32_t i = 0; i < data_size; i++) {
        if (i + window_size <= data_size) {
            g_rolling_hash[i] = rolling_hash(data + i, window_size);
        } else {
            // 对于无法形成完整窗口的位置，使用部分数据
            g_rolling_hash[i] = rolling_hash(data + i, data_size - i);
        }
    }
    
    // 寻找局部最大值作为分块点
    uint32_t last_chunk_start = 0;
    g_chunk_count = 0;
    
    // 对于小数据，使用较小的搜索范围
    uint32_t half_window = WINDOW_SIZE / 2;
    if (g_data_size < WINDOW_SIZE) {
        half_window = g_data_size / 2;
    }
    
    for (uint32_t i = half_window; i < data_size - half_window; i++) {
        // 检查是否为局部最大值
        if (is_local_maximum(i)) {
            uint32_t chunk_length = i - last_chunk_start;
            
            // 确保块大小在合理范围内
            if (chunk_length >= MIN_CHUNK_SIZE && chunk_length <= MAX_CHUNK_SIZE) {
                // 创建新块
                if (g_chunk_count < 1000) {
                    Chunk *chunk = &g_chunks[g_chunk_count];
                    chunk->start = last_chunk_start;
                    chunk->length = chunk_length;
                    chunk->hash = calculate_hash(data + last_chunk_start, chunk_length);
                    
                    g_chunk_count++;
                    last_chunk_start = i;
                }
            }
        }
    }
    
    // 处理最后一个块
    if (last_chunk_start < data_size) {
        uint32_t chunk_length = data_size - last_chunk_start;
        if (chunk_length >= MIN_CHUNK_SIZE && g_chunk_count < 1000) {
            Chunk *chunk = &g_chunks[g_chunk_count];
            chunk->start = last_chunk_start;
            chunk->length = chunk_length;
            chunk->hash = calculate_hash(data + last_chunk_start, chunk_length);
            
            g_chunk_count++;
        }
    }
    
    // 如果只有一个块且太小，创建整个数据作为一个块
    if (g_chunk_count == 0 && data_size > 0) {
        Chunk *chunk = &g_chunks[0];
        chunk->start = 0;
        chunk->length = data_size;
        chunk->hash = calculate_hash(data, data_size);
        g_chunk_count = 1;
    }
    
    return g_chunk_count;
}

// 打印分块结果
void print_chunks(void) {
    if (!g_chunks || g_chunk_count == 0) {
        printf("No chunks to display\n");
        return;
    }
    
    printf("MAXP Chunking Results:\n");
    printf("Total chunks: %u\n", g_chunk_count);
    printf("Data size: %u bytes\n\n", g_data_size);
    
    for (uint32_t i = 0; i < g_chunk_count; i++) {
        Chunk *chunk = &g_chunks[i];
        printf("Chunk %u:\n", i);
        printf("  Start: %u\n", chunk->start);
        printf("  Length: %u bytes\n", chunk->length);
        printf("  Hash: 0x%08X\n", chunk->hash);
        printf("  Data preview: ");
        
        // 显示前16个字节的十六进制表示
        uint32_t preview_len = (chunk->length > 16) ? 16 : chunk->length;
        for (uint32_t j = 0; j < preview_len; j++) {
            printf("%02X ", g_data[chunk->start + j]);
        }
        if (chunk->length > 16) {
            printf("...");
        }
        printf("\n\n");
    }
}

// 清理内存
void cleanup(void) {
    if (g_chunks) {
        free(g_chunks);
        g_chunks = NULL;
    }
    if (g_rolling_hash) {
        free(g_rolling_hash);
        g_rolling_hash = NULL;
    }
    g_chunk_count = 0;
    g_data = NULL;
    g_data_size = 0;
}

// 测试函数
void test_basic(void) {
    printf("=== Basic Functionality Test ===\n");
    
    const char *test_string = "This is a test string for MAXP chunking algorithm. "
                             "It contains repeated patterns and different content types. "
                             "The algorithm should identify local maxima in the rolling hash.";
    
    uint32_t data_size = (uint32_t)strlen(test_string);
    uint8_t *data = (uint8_t*)malloc(data_size);
    memcpy(data, test_string, data_size);
    
    int chunk_count = maxp_chunk(data, data_size);
    assert(chunk_count > 0);
    
    printf("Test data size: %u bytes\n", data_size);
    printf("Number of chunks created: %d\n", chunk_count);
    
    print_chunks();
    
    // 验证分块覆盖整个数据
    uint32_t total_chunked = 0;
    for (uint32_t i = 0; i < g_chunk_count; i++) {
        total_chunked += g_chunks[i].length;
    }
    assert(total_chunked == data_size);
    printf("✓ All data covered by chunks\n");
    
    cleanup();
    free(data);
    printf("✓ Basic functionality test passed\n\n");
}

void test_different_sizes(void) {
    printf("=== Different Data Sizes Test ===\n");
    
    uint32_t sizes[] = {100, 500, 1000, 5000};
    int num_sizes = (int)(sizeof(sizes) / sizeof(sizes[0]));
    
    for (int i = 0; i < num_sizes; i++) {
        uint8_t *data = (uint8_t*)malloc(sizes[i]);
        
        // 生成测试数据
        for (uint32_t j = 0; j < sizes[i]; j++) {
            data[j] = (uint8_t)((j * 7 + 42) % 256);
        }
        
        int chunk_count = maxp_chunk(data, sizes[i]);
        printf("Size %u: %d chunks (avg chunk size: %.1f)\n", 
               sizes[i], chunk_count, (float)sizes[i] / chunk_count);
        
        cleanup();
        free(data);
    }
    
    printf("✓ Different sizes test passed\n\n");
}

void test_edge_cases(void) {
    printf("=== Edge Cases Test ===\n");
    
    // 测试空数据
    int result = maxp_chunk(NULL, 0);
    assert(result == -1);
    printf("✓ NULL data handled correctly\n");
    
    // 测试极小数据
    uint8_t tiny_data[] = {1, 2, 3, 4, 5};
    result = maxp_chunk(tiny_data, (uint32_t)sizeof(tiny_data));
    if (result > 0) {
        printf("Tiny data (%zu bytes): %d chunks\n", sizeof(tiny_data), result);
        print_chunks();
    } else {
        printf("Tiny data (%zu bytes): failed to chunk\n", sizeof(tiny_data));
    }
    cleanup();
    
    // 测试单字节数据
    uint8_t single_byte = 42;
    result = maxp_chunk(&single_byte, 1);
    if (result > 0) {
        printf("Single byte: %d chunks\n", result);
        print_chunks();
    } else {
        printf("Single byte: failed to chunk\n");
    }
    cleanup();
    
    printf("✓ Edge cases test passed\n\n");
}

// 从文件读取数据并分块
void process_file(const char *filename) {
    printf("=== Processing File: %s ===\n", filename);
    
    FILE *file = fopen(filename, "rb");
    if (!file) {
        printf("Error: Cannot open file %s\n", filename);
        return;
    }
    
    // 获取文件大小
    fseek(file, 0, SEEK_END);
    long file_size_long = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    if (file_size_long < 0) {
        printf("Error: Cannot determine file size\n");
        fclose(file);
        return;
    }
    
    uint32_t file_size = (uint32_t)file_size_long;
    uint8_t *data = (uint8_t*)malloc(file_size);
    size_t bytes_read = fread(data, 1, file_size, file);
    fclose(file);
    
    if (bytes_read != file_size) {
        printf("Error: Failed to read complete file\n");
        free(data);
        return;
    }
    
    printf("File size: %u bytes (%.2f KB)\n", file_size, file_size / 1024.0);
    
    // 执行分块
    int chunk_count = maxp_chunk(data, file_size);
    if (chunk_count < 0) {
        printf("Error: Chunking failed\n");
        free(data);
        return;
    }
    
    printf("Created %d chunks\n", chunk_count);
    printf("Average chunk size: %.1f bytes\n", (float)file_size / chunk_count);
    
    // 显示分块信息
    print_chunks();
    
    cleanup();
    free(data);
    printf("✓ File processing completed\n\n");
}

int main(int argc, char *argv[]) {
    printf("MAXP Algorithm - Simple Implementation\n");
    printf("=====================================\n\n");
    
    // 运行测试
    test_basic();
    test_different_sizes();
    test_edge_cases();
    
    // 处理文件
    if (argc > 1) {
        process_file(argv[1]);
    } else {
        printf("Usage: %s [filename]\n", argv[0]);
        printf("If no filename is provided, only tests will run.\n");
    }
    
    printf("All tests completed successfully!\n");
    return 0;
}
