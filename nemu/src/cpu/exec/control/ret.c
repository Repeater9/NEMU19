#include "cpu/exec/helper.h"

make_helper(ret){
	cpu.eip = swaddr_read(cpu.esp,4) - 1; // 1 for instruction length
	cpu.esp += 4;
	print_asm("ret");
	return 1;
}

make_helper(ret_i){
	uint16_t immidiate = instr_fetch(eip + 1,2);
	cpu.eip = swaddr_read(cpu.esp,4) - 3; // 3 for instruction length
	cpu.esp += immidiate + 4;
	print_asm("ret and pop $0x%x bytes",immidiate);
	return 3;
}
