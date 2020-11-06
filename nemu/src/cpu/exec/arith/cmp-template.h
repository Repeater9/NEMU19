#include "cpu/exec/template-start.h"

#define instr cmp

static void do_execute(){
	DATA_TYPE result = op_dest->val - op_src->val;
	cpu.EFLAGS.CF = result > op_dest->val;
	uint32_t bits = DATA_BYTE * 8;
	uint32_t is_same_sign1 = (op_dest->val ^ op_src->val);
	uint32_t is_same_sign2 = (op_dest->val ^ result);
	cpu.EFLAGS.OF = (is_same_sign1 & is_same_sign2) >> (bits - 1);
	update_eflags_PF_SF_ZF((DATA_TYPE_S)result);
	print_asm_template2();
}

make_instr_helper(i2a)
make_instr_helper(i2rm)
make_instr_helper(r2rm)
make_instr_helper(rm2r)

#include "cpu/exec/template-end.h"

