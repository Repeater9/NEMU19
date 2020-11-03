#include "cpu/exec/template-start.h"

#define instr call

static void do_execute(){
	cpu.esp -= DATA_BYTE;
	swaddr_write(cpu.esp,DATA_BYTE,cpu.eip + DATA_BYTE + 1);
	cpu.eip = cpu.eip + DATA_BYTE + 1 + op_src->simm;
}


#if DATA_BYTE == 4
make_helper(concat(call_rel_,SUFFIX)){
	int len = concat(decode_si_,SUFFIX)(eip + 1);
	do_execute();
	return len + 1;
}

#include "cpu/exec/template-end.h"
