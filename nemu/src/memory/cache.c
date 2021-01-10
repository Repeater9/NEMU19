#include "memory/cache.h"

#include<stdlib.h>

uint32_t dram_read(hwaddr_t, size_t);
void dram_write(hwaddr_t, size_t, uint32_t);

#define exp2(i) ((1 << i));

#define WRITE_THROUGH 0
#define WRITE_BACK 1

Cache L1_cache;
Cache L2_cache;

uint32_t cache_read(Cache *c, hwaddr_t addr, size_t len) {
    uint32_t block_offset = addr & c->offset_mask;

}

uint32_t L1_next_level_read(hwaddr_t addr, size_t len) {
    return cache_read(addr, len, &L2_cache);
}

void L1_next_level_write(hwaddr_t addr, size_t len, uint32_t data) {
    cache_write(addr, len, data, &L2_cache);
}

static void create_cache(Cache *c, int block_size_width, int set_count_width, int associativity,
        int write_policy, uint32_t (* next_level_read) (hwaddr_t, size_t),
        void(* next_level_write)(hwaddr_t, size_t, uint32_t))) {
    
    c->block_size_width = block_size_width;
    c->associativity = associativity;
    c->set_count_width = set_count_width;

    c->block_size = exp2(block_size_width);
    c->set_count = exp2(set_count_width);

    c->set_mask = (exp2(set_count_width) - 1) << block_size_width;
    c->offset_mask = exp2(block_size_width) - 1;
    c->tag_mask = ~(c->set_mask | c->offset_mask);
    
    c->write_policy = write_policy;
    c->next_level_read = next_level_read;
    c->next_level_write = next_level_write;

    int block_count = c->set_count * c->associativity;
    c->line = malloc(block_count * sizeof(Cache));

    int i;
    for(i = 0; i < block_count; i++) {
        c->line[i].valid = 0;
        c->line[i].dirty = 0;
        c->line[i].valid = 0;
        c->line[i].data = malloc(c->block_size);
    }
}

void init_cache(Cache *c)
{
    int i;
    int block_count = c->associativity * c->set_count;
    for(i = 0; i < block_count; i++){
        c->line[i].valid = false;
    }
}

void create_all_cache()
{
    create_cache(&L1_cache, 6, 7, 8, WRITE_THROUGH, L1_next_level_read, L1_next_level_write);
    create_cache(&L2_cache, 6, 12, 16, WRITE_BACK, dram_read, dram_write);
}

void init_all_cache()
{
    init_cache(&L1_cache);
    init_cache(&L2_cache);
}



