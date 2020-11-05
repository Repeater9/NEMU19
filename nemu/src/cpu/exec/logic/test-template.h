#include "cpu/exec/template-start.h"

#define instr test

static void do_execute(){
	DATA_TYPE temp = op_src->val & op_dest->val;
	update_eflags_PF_SF_ZF((DATA_TYPE_S)temp);
	cpu.EFLAGS.CF = 0;
	cpu.EFLAGS.OF = 0;
	print_asm_template2();
}

make_instr_helper(i2rm)
make_instr_helper(r2rm)
make_instr_helper(i2a)


#include "cpu/exec/template-end.h"
