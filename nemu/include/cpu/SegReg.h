#ifndef SegREG_H
#define SegREG_H

#include "common.h"

typedef struct SegmentSelector {
	union {
		/* the visible part */
		struct {
			uint32_t rpl    :2;
			uint32_t ti     :1;
			uint32_t index  :13;
		};
		uint16_t val;
	};
	struct {
		/* the invisible part */
		uint32_t base;
		uint32_t limit;
		uint32_t type				:5;
		uint32_t privilege_level	:2;
		uint32_t soft_use			:1;
	};
} SegSelector;

extern const char* sregs[];

typedef struct TableRegister {
	uint32_t limit      : 16;
	uint32_t base       : 32;
} TableReg;

#endif