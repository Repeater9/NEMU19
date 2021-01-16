#ifndef __CACHE_H__
#define __CACHE_H__

#include "common.h"

typedef struct {
    union {
        union {
            bool valid  : 1;
            uint32_t tag;
        };  
        uint32_t key_val;
    };
    bool dirty;

    uint8_t *sram;
} CacheLine;

#define get_tag(val) ((val) & ~1)

typedef struct {
        int block_size_width;
        int associativity_width;
        int set_size_width;

        int line_size;
        int block_count;
        int associativity;
        int set_count;

        uint32_t tag_mask;
        uint32_t block_mask;
        uint32_t set_mask;
        uint32_t offset_mask;

        int write_policy;
        uint32_t (* next_level_read) (hwaddr_t, size_t);
        void (* next_level_write) (hwaddr_t, size_t, uint32_t);


        CacheLine *line;

        int *last_access;
} Cache;

extern Cache L1_dcache;
extern Cache L2_cache;

uint32_t cache_read(hwaddr_t addr, size_t len, Cache *c);
void cache_write(hwaddr_t addr, size_t len, uint32_t data, Cache *c);

#endif
