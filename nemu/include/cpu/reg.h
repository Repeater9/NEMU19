#ifndef __REG_H__
#define __REG_H__

#include "common.h"

enum { R_EAX, R_ECX, R_EDX, R_EBX, R_ESP, R_EBP, R_ESI, R_EDI };
enum { R_AX, R_CX, R_DX, R_BX, R_SP, R_BP, R_SI, R_DI };
enum { R_AL, R_CL, R_DL, R_BL, R_AH, R_CH, R_DH, R_BH };

/* TODO: Re-organize the `CPU_state' structure to match the register
 * encoding scheme in i386 instruction format. For example, if we
 * access cpu.gpr[3]._16, we will get the `bx' register; if we access
 * cpu.gpr[1]._8[1], we will get the 'ch' register. Hint: Use `union'.
 * For more details about the register encoding scheme, see i386 manual.
 */

typedef struct {
	union{
		union{
			uint32_t _32;
			uint16_t _16;
			uint8_t _8[2];
		}gpr[8];
        /* Do NOT change the order of the GPRs' definitions. */
		struct{
			int eax,ecx,edx,ebx,esp,ebp,esi,edi;
		};
	};

	swaddr_t eip;

	union{
		struct{
			uint32_t CF : 1;
			uint32_t space0 : 1;
			uint32_t PF : 1;
			uint32_t space1 : 1;
			uint32_t AF : 1;
			uint32_t space2 : 1;
			uint32_t ZF : 1;
			uint32_t SF : 1;
			uint32_t TF : 1;
			uint32_t IF : 1;
			uint32_t DF : 1;
			uint32_t OF : 1;
			uint32_t IOPL : 2;
			uint32_t NT : 1;
			uint32_t space3 : 1;
			uint16_t space4;
		};
		uint32_t val;
	}EFLAGS;

}CPU_state;

extern CPU_state cpu;

static inline int check_reg_index(int index) {
	assert(index >= 0 && index < 8);
	return index;
}

#define reg_l(index) (cpu.gpr[check_reg_index(index)]._32)
#define reg_w(index) (cpu.gpr[check_reg_index(index)]._16)
#define reg_b(index) (cpu.gpr[check_reg_index(index) & 0x3]._8[index >> 2])

extern const char* regsl[];
extern const char* regsw[];
extern const char* regsb[];\

void update_eflags_PF_SF_ZF(uint32_t temp);


static inline bool check_cc_a(){
	return !(cpu.EFLAGS.CF | cpu.EFLAGS.ZF);
}

static inline bool check_cc_ae(){
	return !cpu.EFLAGS.CF;
}

static inline bool check_cc_b(){
	return cpu.EFLAGS.CF;
}

static inline bool check_cc_be(){
	return cpu.EFLAGS.CF | cpu.EFLAGS.ZF;
}

static inline bool check_cc_e(){
	return cpu.EFLAGS.ZF;
}

static inline bool check_cc_g(){
	return (cpu.EFLAGS.ZF == 0 && cpu.EFLAGS.SF == cpu.EFLAGS.OF);
}

static inline bool check_cc_ge(){
	return cpu.EFLAGS.SF == cpu.EFLAGS.OF;
}

static inline bool check_cc_l(){
	return cpu.EFLAGS.SF != cpu.EFLAGS.OF;
}

static inline bool check_cc_le(){
	return cpu.EFLAGS.ZF == 1 || cpu.EFLAGS.SF != cpu.EFLAGS.OF;
}

static inline bool check_cc_ne(){
	return !cpu.EFLAGS.ZF;
}

static inline bool check_cc_no(){
	return !cpu.EFLAGS.OF;
}

static inline bool check_cc_np(){
	return !cpu.EFLAGS.PF;
}

static inline bool check_cc_ns(){
	return !cpu.EFLAGS.SF;
}

static inline bool check_cc_o(){
	return cpu.EFLAGS.OF; 
}

static inline bool check_cc_p(){
	return cpu.EFLAGS.PF;
}

static inline bool check_cc_s(){
	return cpu.EFLAGS.SF;	
}


#endif
