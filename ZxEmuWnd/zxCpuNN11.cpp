#include "stdafx.h"
#include "zxCPU.h"

funcCPU table_opsN11_XXX[] = {
	&zxCPU::funcN11_000, &zxCPU::funcN11_001, &zxCPU::funcN11_000, &zxCPU::funcN11_011, &zxCPU::funcN11_000,
	&zxCPU::funcN11_101, &zxCPU::funcN11_000, &zxCPU::funcN11_000
};

int zxCPU::funcN11_000() {
	static int tt[] = { 5, 0, 10, 0, 10, 0, 7, 11};
	ssh_w nn = 0;
	if(typeOps < 6) {
		if(typeOps > 0) nn = read_mem16PC();
		if(!isFlag(ops)) return tt[typeOps];
	}
	switch(typeOps) {
		// RET CCC; 11/5
		case 0: (*_PC) = read_mem16(*_SP); (*_SP) += 2; return 11;
		// JP CCC, nn 10/10
		case 2:  (*_PC) = nn; return 10;
		// CALL ССС, nn 17/10
		// RST NNN 11
		// запомнить ПС для операции отладчика шаг с выходом
		case 4: *_PC_EXIT_CALL = (*_PC);
		case 7: (*_SP) -= 2; write_mem16(*_SP, (*_PC)); (*_PC) = (typeOps == 4 ? nn : ops * 8); return (typeOps == 4 ? 17 : 11);
		case 6: return opsAccum(read_mem8PC());
	}
	return 0;
}

int zxCPU::funcN11_001() {
	ssh_w reg;
	if(ops == 5 || ops == 7) {
		reg = *fromRP_AF(4);
	} else if(ops != 3) {
		reg = read_mem16(*_SP);
		(*_SP) += 2;
	}
	switch(ops) {
		// RET 10
		// JP [HL/IX] 4
		case 1:	case 5: (*_PC) = reg; return (ops == 1 ? 10 : 4);
		// EXX 4
		case 3: swapReg(_BC, _BC + 4); swapReg(_DE, _DE + 4); swapReg(_HL, _HL + 4); return 4;
		// LD SP, HL 6
		case 7: *_SP = reg; return 6;
	}
	// POP RP
	*fromRP_AF(ops) = reg;
	return 10;
}

int zxCPU::funcN11_101() {
	switch(ops) {
		// CALL nn
		case 1: return execCALL(read_mem16PC());
		// prefix DD/FD
		case 3: case 7: {
			ssh_b n = read_mem8((*_PC));
			if(n == 0xDD || n == 0xFD || n == 0xED) return noni();
			else execOps(((ops & 4) >> 2) + 1, (n == 0xCB ? PREFIX_CB : 0));
			break;
		}
		// prefix ED
		case 5: execOps(0, PREFIX_ED); break;
	}
	// PUSH RP
	(*_SP) -= 2; write_mem16(*_SP, *fromRP_AF(ops));
	return 11;
}

int zxCPU::funcN11_011() {
	ssh_b a = *_A;
	switch(ops) {
		// JP nn
		case 0: (*_PC) = read_mem16PC(); return 10;
		// prefix CB
		case 1: execOps(0, PREFIX_CB); break;
		// OUT(d), A
		case 2: theApp->bus.writePort(read_mem8PC(), a, a); return 11;
		// IN A, (d)
		case 3: *_A = theApp->bus.readPort(read_mem8PC(), a); return 11;
		// EX (SP), HL
		case 4: {
			ssh_w* reg = fromRP_SP(4);
			ssh_w nn = read_mem16(*_SP);
			write_mem16(*_SP, *reg);
			*reg = nn;
			return 19;
		}
		// EX HL, DE
		case 5: swapReg(_DE, _HL); return 4;
	}
	// EI/DI
	*_IFF1 = *_IFF2 = (ops & 1); modifyTSTATE(!(ops & 1), ZX_INT);
	return 4;
}
