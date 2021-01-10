#ifndef __CACHE_H__
#define __CACHE_H__

#include"common.h"

typedef struct{
    bool valid;
    bool dirty;
    uint32_t tag;
    uint8_t *data;
} CacheLine;

typedef struct{

    int block_size_width;
    int set_count_width;

    int block_size;
    int set_count;
    int associativity;

    uint32_t tag_mask;
    uint32_t set_mask;
    uint32_t offset_mask;

    int write_policy;
    uint32_t (* next_level_read) (hwaddr_t, size_t);
    void (* next_level_write) (hwaddr_t, size_t, uint32_t);

    CacheLine *line;
} Cache;

extern Cache L1_cache;
extern Cache L2_cache;

uint32_t cache_read(Cache *c, hwaddr_t addr, size_t len);
uint32_t cache_write(Cache *c, hwaddr_t addr, size_t len, uint32_t val);

#endif