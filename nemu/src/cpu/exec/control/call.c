#include "cpu/exec/helper.h"

make_helper(call_rel_32)
{
	int len = decode_si_l(eip + 1);
	swaddr_t next_addr = eip + 1 + len;
	swaddr_write(cpu.esp - 4,4,next_addr);
	cpu.esp -= 4;
	cpu.eip = cpu.eip + op_src->simm;
	print_asm("call 0x%x",cpu.eip);
	return len + 1;
}
