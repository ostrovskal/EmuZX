#include "stdafx.h"
#include "zxCPU.h"

funcCPU table_opsN11_XXX[] = {
	&zxCPU::funcN11_000, &zxCPU::funcN11_001, &zxCPU::funcN11_000, &zxCPU::funcN11_011, &zxCPU::funcN11_000,
	&zxCPU::funcN11_101, &zxCPU::funcN11_000, &zxCPU::funcN11_000
};

void zxCPU::funcN11_000() {
	ssh_w nn = 0;
	if(typeOps < 6) {
		if(typeOps > 0) nn = read_mem16PC();
		if(!isFlag(ops)) return;
	}
	switch(typeOps) {
		// RET CCC
		case 0: (*_PC) = read_mem16(*_SP); (*_SP) += 2; break;
		// JP CCC, nn
		case 2:  (*_PC) = nn; break;
		// CALL ССС, nn
		// RST NNN
		// запомнить ПС для операции отладчика шаг с выходом
		case 4: *_PC_EXIT_CALL = (*_PC);
		case 7:
			(*_SP) -= 2;
			write_mem16(*_SP, (*_PC));
			(*_PC) = (typeOps == 4 ? nn : ops * 8);
			break;
		// ARIFTH
		case 6:  opsAccum(read_mem8PC()); break;
	}
}

void zxCPU::funcN11_001() {
	ssh_w reg;
	if(ops == 5 || ops == 7) {
		reg = *fromRP_AF(4);
	} else if(ops != 3) {
		reg = read_mem16(*_SP);
		(*_SP) += 2;
	}
	switch(ops) {
		// RET
		// JP [HL/IX]
		case 1:	case 5: (*_PC) = reg; break;
		// EXX
		case 3: swapReg(_BC, _BC + 4); swapReg(_DE, _DE + 4); swapReg(_HL, _HL + 4); break;
		// LD SP, HL
		case 7: *_SP = reg; break;
		// POP RP
		default: *fromRP_AF(ops) = reg;
	}
}

void zxCPU::funcN11_101() {
	switch(ops) {
		// CALL nn
		case 1: execCALL(read_mem16PC()); break;
		// prefix DD/FD
		case 3: case 7: {
			ssh_b n = read_mem8((*_PC));
			if(n == 0xDD || n == 0xFD || n == 0xED) noni();
			else execOps(((ops & 4) >> 2) + 1, (n == 0xCB ? PREFIX_CB : 0));
			break;
		}
		// prefix ED
		case 5: execOps(0, PREFIX_ED); break;
		// PUSH RP
		default: (*_SP) -= 2; write_mem16(*_SP, *fromRP_AF(ops));
	}
}

void zxCPU::funcN11_011() {
	ssh_b a = *_A;
	switch(ops) {
		// JP nn
		case 0: (*_PC) = read_mem16PC(); break;
		// prefix CB
		case 1: execOps(0, PREFIX_CB); break;
		// OUT(d), A
		case 2: writePort(read_mem8PC(), a, a); break;
		// IN A, (d)
		case 3: *_A = readPort(read_mem8PC(), a); break;
		// EX (SP), HL
		case 4: {
			ssh_w* reg = fromRP_SP(4);
			ssh_w nn = read_mem16(*_SP);
			write_mem16(*_SP, *reg);
			*reg = nn;
			break;
		}
		// EX HL, DE
		case 5: swapReg(_DE, _HL); break;
		// EI/DI
		default: *_IFF1 = *_IFF2 = (ops & 1); modifyTSTATE(!(ops & 1), ZX_INT);
	}
}
