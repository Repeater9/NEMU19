// #ifndef __CACHE_H__
// #define __CACHE_H__

// #include"common.h"

// typedef struct{
//     bool valid;
//     bool dirty;
//     uint32_t tag;
//     uint8_t *data;
// } CacheLine;

// typedef struct{

//     uint32_t block_size_width;
//     uint32_t set_count_width;

//     uint32_t block_size;
//     uint32_t set_count;
//     uint32_t associativity;

//     uint32_t tag_mask;
//     uint32_t set_mask;
//     uint32_t offset_mask;

//     int write_policy;
//     uint32_t (* next_level_read) (hwaddr_t, size_t);
//     void (* next_level_write) (hwaddr_t, size_t, uint32_t);

//     CacheLine *line;
//     uint32_t *lately_visit;                   //record the block that we lately use in a set

// } Cache;

// extern Cache L1_cache;
// extern Cache L2_cache;

// uint32_t cache_read(Cache *c, hwaddr_t addr, size_t len);
// void cache_write(Cache *c, hwaddr_t addr, size_t len, uint32_t val);

// #endif

#ifndef __CACHE_H__
#define __CACHE_H__

#include "common.h"

typedef struct {
        union {
                union {
                        bool valid      : 1;
                        uint32_t tag;
                };
                uint32_t key_val;
        };
        bool dirty;

        uint8_t *sram;
} CacheLine;

#define CL_VALID 1
#define get_tag(val) ((val) & ~CL_VALID)

typedef struct {
        int line_size_width;
        int associativity_width;
        int set_size_width;

        int line_size;
        int nr_line;
        int associativity;
        int nr_set;

        uint32_t tag_mask;
        uint32_t line_mask;
        uint32_t set_mask;
        uint32_t offset_mask;

        int write_policy;
        void (* coherency) (hwaddr_t, size_t);
        uint32_t (* next_level_read) (hwaddr_t, size_t);
        void (* next_level_write) (hwaddr_t, size_t, uint32_t);


        CacheLine *line;

        int *last_access;
} Cache;

extern Cache L1_icache;
extern Cache L1_dcache;
extern Cache L2_cache;

uint32_t cache_read(hwaddr_t addr, size_t len, Cache *c);
void cache_write(hwaddr_t addr, size_t len, uint32_t data, Cache *c);

#endif
