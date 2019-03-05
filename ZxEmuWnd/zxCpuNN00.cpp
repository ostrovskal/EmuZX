#include "stdafx.h"
#include "zxCPU.h"

funcCPU table_opsN00_XXX[] = {
	&zxCPU::funcN00_000, &zxCPU::ldRp, &zxCPU::funcN00_010, &zxCPU::incRp, &zxCPU::incSSS, &zxCPU::incSSS, &zxCPU::ldSSS, &zxCPU::funcN00_111
};

int zxCPU::funcN00_000() {
	int d = (ops > 1 ? (char)read_mem8PC() : 0);
	switch(ops) {
		// NOP
		case 0: return 4;
		// EX AF, AF'
		case 1: swapReg(_AF, _AF + 4); return 4;
		// DJNZ
		case 2: (*_B)--; if(*_B) { (*_PC) += d; return 13; } return 8;
		// JR
		case 3: (*_PC) += d; return 12;
	}
	// JR CCC
	if(isFlag(ops & 3)) { 
		(*_PC) += d;
		return 12; 
	}
	return 7;
}

int zxCPU::funcN00_010() {
	if(ops < 4) {
		// LD (BC/DE), A | LD A, (BC/DE)
		ssh_w reg = *fromRP_AF(ops);
		if(ops & 1) *_A = read_mem8(reg); else write_mem8(reg, *_A);
		return 7;
	}
	// LD [nn], HL/A | LD A/HL, [nn]
	ssh_w nn = read_mem16PC();
	ssh_w* reg = fromRP_AF(4);
	bool d = ops & 2;
	if(ops & 1) {
		if(d) *_A = read_mem8(nn); else *reg = read_mem16(nn);
	} else {
		if(d) write_mem8(nn, *_A); else write_mem16(nn, *reg);
	}
	return (d ? 13 : 16);
}

int zxCPU::funcN00_111() {
	ssh_b a = *_A;
	ssh_b oldFc = getFlag(FC);
	switch(ops) {
		// DAA SZ5*3P-*
		case 4: {
			ssh_b oldA = a;
			ssh_b fh = getFlag(FH);
			ssh_b fn = getFlag(FN);
			ssh_b fc = 0;
			if(((a & 15) > 9) || fh) {
				ssh_w val = a + (fn ? -6 : 6);
				a = (ssh_b)val;
				fc = oldFc | (val > 255 ? 1 : 0);
				fh = 16;
			} else fh = 0;
			if((oldA > 0x99) || oldFc) {
				a += (fn ? -96 : 96);
				fc = 1;
			} else fc = 0;
			update_flags(FS | FZ | FH | FPV | FC, _FS(a) | _FZ(a) | fh | _FP(a) | fc);
			break;
		}
		// CPL; --*1*-1-
		case 5: a = ~a; update_flags(FH | FN, 18); break;
		// SCF; --*0*-01
		case 6:	update_flags(FH | FN | FC, 1); break;
		// CCF; --***-0C H <-старый C
		case 7: update_flags(FH | FN | FC, (oldFc << 4) | (oldFc == 0)); break;
		default: a = rotate(a, FH | FN | FC);
	}
	*_A = a;
	return 4;
}

int zxCPU::ldRp() {
	ssh_w* reg = fromRP_SP(ops);
	if(ops & 1) {
		// ADD HL, RP; --***-0C	F5,H,F3 берутся по результатам сложения старших байтов
		ssh_w* hl = fromRP_SP(4);
		ssh_w valHL = *hl;
		ssh_d val = valHL + *reg;
		*hl = (ssh_w)val;
		update_flags(FH | FN | FC, _FH(valHL >> 8, (*reg) >> 8, 0, 0) | (val > 65535));
		return 11;
	}
	// LD RP, NN
	*reg = read_mem16PC();
	return 10;
}

int zxCPU::incRp() { 
	*fromRP_SP(ops) += ((ops & 1) ? -1 : 1);
	return 6;
}

int zxCPU::incSSS() {
	// SZ5H3VN-
	ssh_b n = typeOps & 1;
	ssh_b* reg = fromPrefRON(ops);
	ssh_b op1 = *reg;
	ssh_b op2 = (n ? -1 : 1);
	ssh_b val = op1 + op2;
	::write_mem8(reg, val);
	update_flags(FS | FZ | FH | FPV | FN, _FS(val) | _FZ(val) | _FH(op1, op2, 0, n) | _FV1(op1, op2, 0, n) | _FN(n));
	return (ops == 6 ? 11 : 4);
}

int zxCPU::ldSSS() {
	ssh_b* reg = fromPrefRON(ops);
	::write_mem8(reg, read_mem8PC());
	return 7;
}
