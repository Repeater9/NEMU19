#include "cpu/exec/template-start.h"

#define make_setcc_helper(cc) \
	make_helper(concat(set,cc)){ \
		int len = decode_rm_b(eip + 1);\
		uint32_t result = concat(check_cc_,cc)(); \
		OPERAND_W(op_src,result); \
		print_asm(str(concat(set,cc)) " %s", op_src->str); \
		return len + 1;\
	}

make_setcc_helper(a)
make_setcc_helper(ae)
make_setcc_helper(b)
make_setcc_helper(be)
make_setcc_helper(e)
make_setcc_helper(g)
make_setcc_helper(ge)
make_setcc_helper(l)
make_setcc_helper(le)
make_setcc_helper(ne)
make_setcc_helper(no)
make_setcc_helper(np)
make_setcc_helper(ns)
make_setcc_helper(o)
make_setcc_helper(p)
make_setcc_helper(s)

#include "cpu/exec/template-end.h"

