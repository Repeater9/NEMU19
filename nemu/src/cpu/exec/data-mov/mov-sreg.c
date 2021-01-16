#include "cpu/exec/helper.h"

void load_sreg(uint8_t, uint16_t);

make_helper(mov_rm2sreg_w) {
	int len = decode_rm2r_w(eip + 1);
	uint8_t sreg = op_dest->reg;
	load_sreg(sreg, op_src->val);
	print_asm("movw %s,%%%s", op_src->str, sregs[sreg]);
	return len + 1;
}

