/*
 *csim.c-使用C编写一个Cache模拟器，它可以处理来自Valgrind的跟踪和输出统计
 *息，如命中、未命中和逐出的次数。更换政策是LRU。
 * 设计和假设:
 *  1.
 * 每个加载/存储最多可导致一个缓存未命中。（最大请求是8个字节。）
 *  2.
 * 忽略指令负载（I），因为我们有兴趣评估trace.c内容中数据存储性能。
 *  3.
 * 数据修改（M）被视为加载，然后存储到同一地址。因此，M操作可能导致两次缓存命中，或者一次未命中和一次命中，外加一次可能的逐出。
 * 使用函数printSummary() 打印输出，输出hits, misses and evictions
 * 的数，这对结果评估很重要
 */
#include <getopt.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "cachelab.h"

//****************************Begin*********************
#define TRUE 1
#define FALSE 0

typedef struct cache_line_t {
  char valid;
  unsigned long long tag;
  unsigned long long lru;
} cache_line;

typedef cache_line *cache_set; // cache组
typedef cache_set *cache;      // cache定义

// globals
int hit_count = 0;                  // 命中计数
int miss_count = 0;                 // 未命中计数
int eviction_count = 0;             // 覆盖计数
unsigned long long set_index_mask;  // 组索引掩码
unsigned long long lru_counter = 0; // lru计数器

int s = 0;
int b = 0;
int S = 0; // 组数
int B = 0; // 块大小
int E = 0; // 行数

int verbose = 0;

cache _cache; // cache

void initCache() {
  _cache = (void *)malloc(sizeof(cache_set) * S);
  for (int i = 0; i < S; i++) {
    _cache[i] = (void *)malloc(sizeof(cache_line) * E);
    for (int j = 0; j < E; j++) {
      _cache[i][j].valid = FALSE;
      _cache[i][j].lru = 0;
      _cache[i][j].tag = 0;
    }
  }
  set_index_mask = (1 << s) - 1;
}

void freeCache() {
  for (int i = 0; i < S; i++) {
    free(_cache[i]);
  }
  free(_cache);
}

void accessData(unsigned long long addr) {
  unsigned long long tag = addr >> (s + b);
  unsigned long long set_idx = (addr >> b) & set_index_mask;
  int first_free = -1;
  for (int i = 0; i < E; i++) {
    if ((_cache[set_idx][i].tag == tag) && (_cache[set_idx][i].valid == TRUE)) {
      hit_count++;
      _cache[set_idx][i].lru = lru_counter;
      lru_counter++;
      if (verbose)
        printf("hit ");
      return;
    } else if (_cache[set_idx][i].valid == FALSE && first_free == -1) {
      first_free = i;
    }
  }
  miss_count++;
  if (first_free != -1) {
    _cache[set_idx][first_free].tag = tag;
    _cache[set_idx][first_free].valid = TRUE;
    _cache[set_idx][first_free].lru = lru_counter;
    lru_counter++;
    if (verbose)
      printf("miss ");
    return;
  }

  unsigned long long min_cnt = ULLONG_MAX;
  int min_cnt_idx = -1;
  for (int i = 0; i < E; i++) {
    if (_cache[set_idx][i].lru < min_cnt) {
      min_cnt_idx = i;
      min_cnt = _cache[set_idx][i].lru;
    }
  }
  _cache[set_idx][min_cnt_idx].lru = lru_counter;
  lru_counter++;
  _cache[set_idx][min_cnt_idx].tag = tag;
  eviction_count++;
  if (verbose)
    printf("eviction ");
}

void replayTrace(const char *filename) {
  char buf[100];
  unsigned long long addr = 0;
  unsigned long long len = 0;
  FILE *fp = fopen(filename, "r");
  if (fp == NULL) {
    return;
  }

  while (fgets(buf, 100, fp) != NULL) {
    if (buf[1] == 'L' || buf[1] == 'S' || buf[1] == 'M') {
      sscanf(buf + 3, "%llx,%llu", &addr, &len);
    } else if (buf[0] == 'I') {
      continue;
    }
    if (verbose)
      printf("%c %llx,%llu", buf[1], addr, len);
    accessData(addr);
    if (buf[1] == 'M')
      accessData(addr);
    printf("\n");
  }

  fclose(fp);
}

void printUsage(char *argv[]) {
  printf("Usage: %s [-hv] -s <num> -E <num> -b <num> -t <file>\n", argv[0]);
  printf("Options:\n");
  printf("  -h         Print this help message.\n");
  printf("  -v         Optional verbose flag.\n");
  printf("  -s <num>   Number of set index bits.\n");
  printf("  -E <num>   Number of lines per set.\n");
  printf("  -b <num>   Number of block offset bits.\n");
  printf("  -t <file>  Trace file.\n");
  printf("\nExamples:\n");
  printf("  linux>  %s -s 4 -E 1 -b 4 -t traces/yi.trace\n", argv[0]);
  printf("  linux>  %s -v -s 8 -E 2 -b 4 -t traces/yi.trace\n", argv[0]);
  exit(0);
}

int main(int argc, char *argv[]) {
  char c;
  char *filename;

  while ((c = getopt(argc, argv, "s:E:b:t:v:h")) != -1) {
    switch (c) {
    case 's':
      s = atoi(optarg);
      break;
    case 'E':
      E = atoi(optarg);
      break;
    case 'b':
      b = atoi(optarg);
      break;
    case 't':
      filename = optarg;
      break;
    case 'v':
      verbose = 1;
      break;
    case 'h':
      printUsage(argv);
      break;
    default:
      break;
    }
  }

  S = 1 << s;
  B = 1 << b;

  initCache();

  replayTrace(filename);
  freeCache();

  printSummary(hit_count, miss_count,
               eviction_count); // �出hit、miss和evictions数量统计
  return 0;
}
//****************************End**********************#
