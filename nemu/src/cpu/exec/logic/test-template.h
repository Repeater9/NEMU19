#include "cpu/exec/template-start.h"

#define instr test

static void do_execute(){
	DATA_TYPE temp = op_src->val & op_dest->val;
	cpu.EFLAGS.OF = cpu.EFLAGS.CF = 0;
	uint8_t temp1 = temp & 0xff;
	uint32_t count = 0;
	uint32_t i;
	for(i = 0;i < 8;i++)
	{
		if(temp1 & 1)
		  count++;
		temp1 = temp1 >> 1;
	}
	if(count & 1)
	  cpu.EFLAGS.PF = 0;
	else cpu.EFLAGS.PF = 1;
	uint32_t bits = DATA_BYTE * 4;
	cpu.EFLAGS.SF = temp >> (bits - 1);
	cpu.EFLAGS.ZF = (temp == 0);
	print_asm_template2();
}

make_instr_helper(i2rm)
make_instr_helper(r2rm)
make_instr_helper(i2a)


#include "cpu/exec/template-end.h"
