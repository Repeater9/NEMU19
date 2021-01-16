#include "cpu/exec/helper.h"

void init_all_tlb();

make_helper(lgdt) {
	int len = decode_rm_l(eip + 1);
	cpu.gdtr.limit = lnaddr_read(op_src->addr, 2);
	cpu.gdtr.base = lnaddr_read(op_src->addr + 2, 4);

	print_asm("lgdt %s", op_src->str);
	return len + 1;
}

make_helper(mov_r2cr) {
	int len = decode_r2rm_l(eip + 1);
	assert(op_dest->type == OP_TYPE_REG);
	if(op_src->reg == 0) {
		uint32_t temp = cpu.cr0.val;
		cpu.cr0.val = reg_l(op_dest->reg);
		if(cpu.cr0.protect_enable) {
			if((temp & 0x1) == 0) {
				cpu.cs.base = 0;
				cpu.cs.limit = 0xffffffff;
				cpu.cs.type = 0x1a;
				cpu.cs.privilege_level = 0;
				cpu.cs.soft_use = 0;
			}
		}
	}

	else if(op_src->reg == 3) {
		cpu.cr3.val = reg_l(op_dest->reg);
		init_all_tlb();
	}
	
	else { assert(0); }

	print_asm("movl %%%s,%%cr%d", regsl[op_dest->reg], op_src->reg);
	return len + 1;
}

make_helper(mov_cr2r) {
	int len = decode_r2rm_l(eip + 1);
	assert(op_dest->type == OP_TYPE_REG);
	if(op_dest->reg == 0) { reg_l(op_src->reg) = cpu.cr0.val; }
	else if(op_dest->reg == 3) { reg_l(op_src->reg) = cpu.cr3.val; }
	else { assert(0); }

	print_asm("movl %%cr%d,%%%s", op_src->reg, regsl[op_dest->reg]);
	return len + 1;
}