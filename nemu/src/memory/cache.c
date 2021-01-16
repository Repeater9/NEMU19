#include "memory/cache.h"

#include<stdlib.h>

uint32_t dram_read(hwaddr_t, size_t);
void dram_write(hwaddr_t, size_t, uint32_t);

#define exp2(i) ((1 << i))

#define WRITE_THROUGH 0
#define WRITE_BACK 1

Cache L1_cache;
Cache L2_cache;

bool is_cache_hit(Cache *c, hwaddr_t addr, CacheLine **hit_line) {
    int set_no = (addr & c->set_mask) >> c->set_count_width;
    CacheLine *cl = c->line + set_no * c->associativity;
    int tag = (addr & c->tag_mask) >> (c->set_count_width + c->block_size_width);
    CacheLine *temp = cl + c->lately_visit[set_no];

    if(temp->tag == tag) {
        // cache hit
        *hit_line = temp;
        return true;
    }

    temp = cl + c->associativity - 1;

    for(; temp >= cl; temp--) {
        if(temp->tag == tag) {
            // cache hit
            *hit_line = temp;
            c->lately_visit[set_no] = temp - cl;
            return true;
        }
    }

    //cache miss
    c->lately_visit[set_no] = rand() & (c->associativity - 1);           // randomly choose a cache to be replaced
    *hit_line = cl + c->lately_visit[set_no];                           // This cache will be replaced
    return false;
}

CacheLine *cache_fetch(Cache *c, hwaddr_t addr) {
    CacheLine *cl;
    if(is_cache_hit(c, addr, &cl)) {
        return cl;
    }

    int i;
    if(cl->valid && cl->dirty) {                                       // In this condition, cache should be written back to next_level
        int set_no = (cl - c->line) / c->associativity;
        uint32_t next_level_start_addr = cl->tag + (set_no << c->block_size_width);

        for(i = 0; i < c->block_size; i+= 4) {
            c->next_level_write(next_level_start_addr + i, 4, unalign_rw(cl->data + i, 4));
        }
    }

    uint32_t block_start = addr & (~c->offset_mask);
    for(i = 0; i < c->block_size; i += 4) {
        unalign_rw(cl->data + i, 4) = c->next_level_read(block_start + i, 4);
    }

    cl->tag = (addr & c->tag_mask) >> (c->set_count_width + c->block_size_width);
    cl->dirty = false;
    cl->valid = true;
    return cl;
}

uint32_t cache_read(Cache *c, hwaddr_t addr, size_t len) {
    uint32_t block_offset = addr & c->offset_mask;
    uint32_t rest_byte = c->block_size - block_offset;
    CacheLine *cl = cache_fetch(c, addr);
    uint8_t *data = cl->data + block_offset;

    if(rest_byte < len) {                       //data cross the boundary
        CacheLine *next_line = cache_fetch(c, addr + c->block_size);
        if(rest_byte == 1)
            return unalign_rw(data, 1) | (unalign_rw(next_line->data, 3) << 8);
        else if(rest_byte == 2)
            return unalign_rw(data, 2) | (unalign_rw(next_line->data, 2) << 16);
        else if(rest_byte == 3)
            return unalign_rw(data, 3) | (unalign_rw(next_line->data, 1) << 24);
    }

    return unalign_rw(data, 4);
}

void cache_write(Cache *c, hwaddr_t addr, size_t len, uint32_t val) {
    uint32_t block_offset = addr & c->offset_mask;
    uint32_t rest_byte = c->block_size - block_offset;
    CacheLine *cl = cache_fetch(c, addr);
    uint8_t *data = cl->data + block_offset;

    if(rest_byte < len) {
        CacheLine *next_line = cache_fetch(c, addr + c->block_size);
        uint8_t *data2 = next_line->data;
        if(rest_byte == 1) {
            unalign_rw(data, 1) = val & 0xff;
            unalign_rw(data2, 3) = (val >> 8) & 0xffffff;
        }
        else if(rest_byte == 2) {
            unalign_rw(data, 2) = val & 0xffff;
            unalign_rw(data2, 2) = (val >> 16) & 0xffff;
        }
        else if(rest_byte == 3) {
            unalign_rw(data, 3) = val & 0xffffff;
            unalign_rw(data2, 1) = (val >> 24) & 0xff;
        }
        
        if(c->write_policy == WRITE_THROUGH) {
            cl->dirty = next_line->dirty = false;
            c->next_level_write(addr & ~0x3, 4, unalign_rw((size_t)data & ~0x3, 4));
            c->next_level_write(addr + rest_byte, 4, unalign_rw(data2, 4));
        }
        else {
            cl->dirty = next_line->dirty = true;
        }
    }

    else {
        if(len == 1) {
            unalign_rw(data, 1) = val & 0xff;
        }
        else if(len == 2) {
            unalign_rw(data, 2) = val & 0xffff;
        }
        else if(len == 3) {
            unalign_rw(data, 3) = val & 0xffffff;
        }
        else if(len == 4) {
            unalign_rw(data, 4) = val;
        }
        if(c->write_policy == WRITE_BACK) {
            cl->dirty = true;
        }
        else {
            cl->dirty = false;
            c->next_level_write(addr, 4, val);
        }
    }
}

uint32_t L1_next_level_read(hwaddr_t addr, size_t len) {
    return cache_read(&L2_cache, addr, len);
}

void L1_next_level_write(hwaddr_t addr, size_t len, uint32_t val) {
    cache_write(&L2_cache, addr, len, val);
}

static void create_cache(Cache *c, int block_size_width, int set_count_width, int associativity,
        int write_policy, uint32_t (* next_level_read) (hwaddr_t, size_t),
        void(* next_level_write)(hwaddr_t, size_t, uint32_t)) {
    
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
    c->lately_visit = malloc(c->set_count * sizeof(int));

    int i;
    for(i = 0; i < block_count; i++) {
        c->line[i].valid = false;
        c->line[i].dirty = false;
        c->line[i].tag = 0;
        c->line[i].data = malloc(c->block_size);
    }

    for(i = 0; i < c->set_count; i++) {
        c->lately_visit[i] = 0;
    }
}

void init_cache(Cache *c)
{
    int i;
    int block_count = c->associativity * c->set_count;
    for(i = 0; i < block_count; i++) {
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



