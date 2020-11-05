#include "cpu/exec/helper.h"

make_helper(call_rel_32)
{
	int len = decode_si_l(eip + 1);
	swaddr_t next_addr = eip + 1 + len;
	swaddr_write(cpu.esp - 4,4,next_addr);
	cpu.esp -= 4;
	cpu.eip = cpu.eip + op_src->simm;
	/*after calling call_rel_32 cpu.eip will be added 5 in cpu-exec()                             here we don't need to add 5*/
	print_asm("call (0x%x)",cpu.eip + len + 1);
	return len + 1;
}
