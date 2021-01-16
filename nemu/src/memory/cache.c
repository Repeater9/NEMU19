// #include "memory/cache.h"

// #include<stdlib.h>

// uint32_t dram_read(hwaddr_t, size_t);
// void dram_write(hwaddr_t, size_t, uint32_t);

// #define exp2(i) ((1 << i))

// #define WRITE_THROUGH 0
// #define WRITE_BACK 1

// Cache L1_cache;
// Cache L2_cache;

// bool is_cache_hit(Cache *c, hwaddr_t addr, CacheLine **hit_line) {
//     uint32_t set_no = (addr & c->set_mask) >> c->set_count_width;
//     CacheLine *cl = c->line + set_no * c->associativity;
//     uint32_t tag = (addr & c->tag_mask) >> (c->set_count_width + c->block_size_width);
//     CacheLine *temp = cl + c->lately_visit[set_no];

//     if(temp->tag == tag) {
//         // cache hit
//         *hit_line = temp;
//         return true;
//     }

//     temp = cl + c->associativity - 1;

//     for(; temp >= cl; temp--) {
//         if(temp->tag == tag) {
//             // cache hit
//             *hit_line = temp;
//             c->lately_visit[set_no] = temp - cl;
//             return true;
//         }
//     }

//     //cache miss
//     c->lately_visit[set_no] = rand() & (c->associativity - 1);           // randomly choose a cache to be replaced
//     *hit_line = cl + c->lately_visit[set_no];                           // This cache will be replaced
//     return false;
// }

// CacheLine *cache_fetch(Cache *c, hwaddr_t addr) {
//     CacheLine *cl;
//     if(is_cache_hit(c, addr, &cl)) {
//         return cl;
//     }

//     int i;
//     if(cl->valid && cl->dirty) {                                       // In this condition, cache should be written back to next_level
//         uint32_t set_no = (cl - c->line) / c->associativity;
//         uint32_t next_level_start_addr = cl->tag + (set_no << c->block_size_width);

//         for(i = 0; i < c->block_size; i+= 4) {
//             c->next_level_write(next_level_start_addr + i, 4, unalign_rw(cl->data + i, 4));
//         }
//     }

//     uint32_t block_start = addr & (~(c->offset_mask));
//     for(i = 0; i < c->block_size; i += 4) {
//         unalign_rw(cl->data + i, 4) = c->next_level_read(block_start + i, 4);
//     }

//     cl->tag = (addr & c->tag_mask) >> (c->set_count_width + c->block_size_width);
//     cl->dirty = false;
//     cl->valid = true;
//     return cl;
// }

// uint32_t cache_read(Cache *c, hwaddr_t addr, size_t len) {
//     uint32_t block_offset = addr & c->offset_mask;
//     uint32_t rest_byte = c->block_size - block_offset;
//     CacheLine *cl = cache_fetch(c, addr);
//     uint8_t *data = cl->data + block_offset;

//     if(rest_byte < len) {                       //data cross the boundary
//         CacheLine *next_line = cache_fetch(c, addr + c->block_size);
//         if(rest_byte == 1)
//             return unalign_rw(data, 1) | (unalign_rw(next_line->data, 3) << 8);
//         else if(rest_byte == 2)
//             return unalign_rw(data, 2) | (unalign_rw(next_line->data, 2) << 16);
//         else if(rest_byte == 3)
//             return unalign_rw(data, 3) | (unalign_rw(next_line->data, 1) << 24);
//     }

//     return unalign_rw(data, 4);
// }

// void cache_write(Cache *c, hwaddr_t addr, size_t len, uint32_t val) {
//     uint32_t block_offset = addr & c->offset_mask;
//     uint32_t rest_byte = c->block_size - block_offset;
//     CacheLine *cl = cache_fetch(c, addr);
//     uint8_t *data = cl->data + block_offset;

//     if(rest_byte < len) {
//         CacheLine *next_line = cache_fetch(c, addr + c->block_size);
//         uint8_t *data2 = next_line->data;
//         if(rest_byte == 1) {
//             unalign_rw(data, 1) = val & 0xff;
//             unalign_rw(data2, 3) = (val >> 8) & 0xffffff;
//         }
//         else if(rest_byte == 2) {
//             unalign_rw(data, 2) = val & 0xffff;
//             unalign_rw(data2, 2) = (val >> 16) & 0xffff;
//         }
//         else if(rest_byte == 3) {
//             unalign_rw(data, 3) = val & 0xffffff;
//             unalign_rw(data2, 1) = (val >> 24) & 0xff;
//         }

//         // uint8_t *p_val = (void *)&val + rest_byte;
// 		// uint32_t rest_byte2 = len - rest_byte;
// 		// if(rest_byte2 -- > 0) {
// 		// 	*data2 = *p_val;
// 		// 	if(rest_byte2 -- > 0) {
// 		// 		*(++ data2) = *(++ p_val);
// 		// 		if(rest_byte2 -- > 0) {
// 		// 			*(++ data2) = *(++ p_val);
// 		// 		}
// 		// 	}
// 		// }
        
//         if(c->write_policy == WRITE_THROUGH) {
//             cl->dirty = next_line->dirty = false;
//             c->next_level_write(addr & ~0x3, 4, unalign_rw((size_t)data & ~0x3, 4));
//             c->next_level_write(addr + rest_byte, 4, unalign_rw(data2, 4));
//         }
//         else {
//             cl->dirty = next_line->dirty = true;
//         }
//     }

//     else {
//         if(len == 1) {
//             unalign_rw(data, 1) = val & 0xff;
//         }
//         else if(len == 2) {
//             unalign_rw(data, 2) = val & 0xffff;
//         }
//         else if(len == 3) {
//             unalign_rw(data, 3) = val & 0xffffff;
//         }
//         else if(len == 4) {
//             unalign_rw(data, 4) = val;
//         }

//         if(c->write_policy == WRITE_BACK) {
//             cl->dirty = true;
//         }
//         else {
//             cl->dirty = false;
//             c->next_level_write(addr, len, val);
//         }
//     }
// }

// uint32_t L1_next_level_read(hwaddr_t addr, size_t len) {
//     return cache_read(&L2_cache, addr, len);
// }

// void L1_next_level_write(hwaddr_t addr, size_t len, uint32_t val) {
//     cache_write(&L2_cache, addr, len, val);
// }

// static void create_cache(Cache *c, int block_size_width, int set_count_width, int associativity,
//         int write_policy, uint32_t (* next_level_read) (hwaddr_t, size_t),
//         void(* next_level_write)(hwaddr_t, size_t, uint32_t)) {
    
//     c->block_size_width = block_size_width;
//     c->associativity = associativity;
//     c->set_count_width = set_count_width;

//     c->block_size = exp2(block_size_width);
//     c->set_count = exp2(set_count_width);

//     c->set_mask = (exp2(set_count_width) - 1) << block_size_width;
//     c->offset_mask = exp2(block_size_width) - 1;
//     c->tag_mask = ~(c->set_mask | c->offset_mask);
    
//     c->write_policy = write_policy;
//     c->next_level_read = next_level_read;
//     c->next_level_write = next_level_write;

//     int block_count = c->set_count * c->associativity;
//     c->line = malloc(block_count * sizeof(Cache));
//     c->lately_visit = malloc(c->set_count * sizeof(int));

//     int i;
//     for(i = 0; i < block_count; i++) {
//         // c->line[i].valid = false;
//         c->line[i].dirty = false;
//         c->line[i].tag = 0;
//         c->line[i].data = malloc(c->block_size);
//     }

//     for(i = 0; i < c->set_count; i++) {
//         c->lately_visit[i] = 0;
//     }
// }

// void init_cache(Cache *c)
// {
//     int i;
//     int block_count = c->associativity * c->set_count;
//     for(i = 0; i < block_count; i++) {
//         c->line[i].valid = false;
//     }
// }

// void create_all_cache()
// {
//     create_cache(&L1_cache, 6, 7, 8, WRITE_THROUGH, L1_next_level_read, L1_next_level_write);
//     create_cache(&L2_cache, 6, 12, 16, WRITE_BACK, dram_read, dram_write);
// }

// void init_all_cache()
// {
//     init_cache(&L1_cache);
//     init_cache(&L2_cache);
// }

#include "memory/cache.h"

#include <stdlib.h>

#define exp2(x) (1 << (x))
#define mask_with_len(x) (exp2(x) - 1)

uint32_t dram_read(hwaddr_t, size_t);
void dram_write(hwaddr_t, size_t, uint32_t);

enum { WRITE_THROUGH, WRITE_BACK };

Cache L1_icache;
Cache L1_dcache;
Cache L2_cache;

//static inline 
bool cache_query(Cache *c, hwaddr_t addr, CacheLine **hit_cl) {
	uint32_t key = (addr & c->tag_mask) | CL_VALID;
	int set = (addr & c->set_mask) >> c->line_size_width;
	CacheLine *cl = c->line + (set << c->associativity_width);
	CacheLine *cl_test = cl + c->last_access[set];

	if(cl_test->key_val == key) {
		// cache hit
		*hit_cl = cl_test;
		return true;
	}

	cl_test = cl + c->associativity - 1;
	
	for(; cl_test >= cl; cl_test --) {
		// check each ways in the set 
		if(cl_test->key_val == key) {
			// cache hit
			*hit_cl = cl_test;
			c->last_access[set] = cl_test - cl;
			return true;
		}
	}

	/* cache miss */
	c->last_access[set] = rand() & mask_with_len(c->associativity_width);
	*hit_cl = cl + c->last_access[set];	// victim cache line
	return false;
}

//static inline
CacheLine* cache_fetch(Cache *c, hwaddr_t addr) {
	CacheLine *cl;
   	if( cache_query(c, addr, &cl) ) {
		/* cache hit */
		return cl;
	}

	/* cache miss */
	int i;
	if (cl->valid && cl->dirty) {
		/* write back */
		int cl_idx = cl - c->line;
		hwaddr_t addr0 = get_tag(cl->key_val) + ((cl_idx >> c->associativity_width) << c->line_size_width);

		for(i = 0; i < c->line_size; i += 4) {
			c->next_level_write(addr0 + i, 4, unalign_rw(cl->sram + i, 4));
		}
	}
	/* cache fill */
	uint32_t line_start_addr = addr & ~c->offset_mask;
	for(i = 0; i < c->line_size; i += 4) {
		unalign_rw(cl->sram + i, 4) = c->next_level_read(line_start_addr + i, 4);
	}

	cl->key_val = (addr & c->tag_mask) | CL_VALID;
	cl->dirty = false;
	return cl;
}

uint32_t cache_read(hwaddr_t addr, size_t len, Cache *c) {
	uint32_t line_offset = addr & c->offset_mask;
	CacheLine *cl = cache_fetch(c, addr);
	uint8_t *p = cl->sram + line_offset;
	uint32_t remain_byte = c->line_size - line_offset;
	if(remain_byte < len) {
		/* data cross the cache line boundary */
		CacheLine *next_cl = cache_fetch(c, addr + c->line_size);
		switch(remain_byte) {  // always be 1, 2, or 3
			case 1: return unalign_rw(p, 1) | (unalign_rw(next_cl->sram, 3) << 8);
			case 2: return unalign_rw(p, 2) | (unalign_rw(next_cl->sram, 2) << 16);
			case 3: return unalign_rw(p, 3) | (unalign_rw(next_cl->sram, 1) << 24);
		}
	}

	return unalign_rw(p, 4);
}

void cache_write(hwaddr_t addr, size_t len, uint32_t data, Cache *c) {
	uint32_t line_offset = addr & c->offset_mask;
	CacheLine *cl = cache_fetch(c, addr);
	uint8_t *p = cl->sram + line_offset;
	uint32_t remain_byte = c->line_size - line_offset;
	if(remain_byte < len) {
		/* data cross the cache line boundary */
		CacheLine *next_cl = cache_fetch(c, addr + c->line_size);
		switch(remain_byte) {  // always be 1, 2, or 3
			case 3: unalign_rw(p + 1, 2) = ((data >> 8) & 0xffff);
			case 1: unalign_rw(p, 1) = data & 0xff; break;
			case 2: unalign_rw(p, 2) = (data & 0xffff); break;
		}

		uint8_t *q = next_cl->sram;
		uint8_t *p_data = (void *)&data + remain_byte;
		uint32_t remain_byte2 = len - remain_byte;
		if(remain_byte2 -- > 0) {
			*q = *p_data;
			if(remain_byte2 -- > 0) {
				*(++ q) = *(++ p_data);
				if(remain_byte2 -- > 0) {
					*(++ q) = *(++ p_data);
				}
			}
		}

		if(c->write_policy == WRITE_THROUGH) {
			cl->dirty = next_cl->dirty = false;
			c->next_level_write(addr & ~0x3, 4, unalign_rw((size_t)p & ~0x3, 4));
			c->next_level_write(addr + remain_byte, 4, unalign_rw(next_cl->sram, 4));
		}
		else {
			cl->dirty = next_cl->dirty = true;
		}
	}
	else {
		switch(len) {
			case 3: unalign_rw(p + 1, 2) = ((data >> 8) & 0xffff);
			case 1: unalign_rw(p, 1) = data & 0xff; break;
			case 2: unalign_rw(p, 2) = (data & 0xffff); break;
			case 4: unalign_rw(p, 4) = data; break;
		}
		cl->dirty = (c->write_policy == WRITE_BACK);
		if(!cl->dirty) {
			c->next_level_write(addr, len, data);
		}
	}

#ifndef ONLY_ONE_L1_CACHE
	if(c->coherency != NULL) {
		c->coherency(addr, len);
	}
#endif
}

static inline uint32_t L1_next_level_read(hwaddr_t addr, size_t len) {
	return cache_read(addr, len, &L2_cache);
}

static inline void L1_next_level_write(hwaddr_t addr, size_t len, uint32_t data) {
	cache_write(addr, len, data, &L2_cache);
}

static inline void coherency(hwaddr_t addr, size_t len) {
	assert(len == 1 || len == 2 || len == 4);
	Cache *c = &L1_icache;

	CacheLine *cl;
   	if( cache_query(c, addr, &cl) ) {
		/* cache hit */
		cl->key_val = 0;
	}

	uint32_t line_offset = addr & c->offset_mask;
	uint32_t remain_byte = c->line_size - line_offset;
	if(remain_byte < len) {
		if( cache_query(c, addr + c->line_size, &cl) ) {
			/* cache hit */
			cl->key_val = 0;
		}
	}
}

static void init_cache(Cache *c) {
	int i;
	for(i = 0; i < c->nr_line; i ++) {
		c->line[i].key_val = 0;
	}
}

static void make_cache(Cache *c, 
		uint32_t tag_mask, uint32_t line_mask, uint32_t set_mask, uint32_t offset_mask,
		int line_size_width, int total_size_width, int associativity_width, 
		int write_policy, void (* coherency) (hwaddr_t, size_t),
		uint32_t (* next_level_read) (hwaddr_t, size_t),
		void (* next_level_write) (hwaddr_t, size_t, uint32_t)) {
	
	c->line_size_width = line_size_width;
	c->associativity_width = associativity_width;
	c->set_size_width = total_size_width - line_size_width - associativity_width;

	c->line_size = exp2(line_size_width);
	c->nr_line = exp2(total_size_width - line_size_width);
	c->associativity = exp2(associativity_width);
	c->nr_set = exp2(total_size_width - line_size_width - associativity_width);

	c->line_mask = (c->nr_line - 1) << line_size_width;
	c->set_mask = mask_with_len(c->set_size_width) << line_size_width;
	c->offset_mask = mask_with_len(line_size_width);
	c->tag_mask = ~(c->set_mask | c->offset_mask);

	c->write_policy = write_policy;
	c->coherency = coherency;
	c->next_level_read = next_level_read;
	c->next_level_write = next_level_write;

	assert(c->associativity <= c->nr_line);

	c->line = malloc(sizeof(CacheLine) * c->nr_line);
	c->last_access = malloc(sizeof(int) * c->nr_set);

	int i;
	for(i = 0; i < c->nr_line; i ++) {
		c->line[i].key_val = 0;
//		c->line[i].valid = false;
		c->line[i].sram = malloc(c->line_size);
	}

	for(i = 0; i < c->nr_set; i ++) {
		c->last_access[i] = 0;
	}
}

void make_all_cache() {
	make_cache(&L1_icache, 
			0xffffe000, 0x0000ffc0, 0x00001fc0, 0x0000003f,
			6, 16, 3, WRITE_THROUGH, NULL, L1_next_level_read, L1_next_level_write);
	make_cache(&L1_dcache, 
			0xffffe000, 0x0000ffc0, 0x00001fc0, 0x0000003f,
			6, 16, 3, WRITE_THROUGH, coherency, L1_next_level_read, L1_next_level_write);
	make_cache(&L2_cache, 
			0xfffc0000, 0x003fffc0, 0x0003ffc0, 0x0000003f,
			6, 22, 4, WRITE_BACK, NULL, dram_read, dram_write);
}

void init_all_cache() {
	init_cache(&L1_icache);
	init_cache(&L1_dcache);
	init_cache(&L2_cache);
}



