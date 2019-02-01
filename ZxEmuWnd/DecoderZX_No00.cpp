#include "stdafx.h"
#include "DecoderZX.h"

funcDecoder table_opsN00_XXX[] = {
	&DecoderZX::funcN00_000, &DecoderZX::ldRp, &DecoderZX::funcN00_010, &DecoderZX::incRp, &DecoderZX::incSSS, &DecoderZX::incSSS, &DecoderZX::ldSSS, &DecoderZX::funcN00_111
};

void DecoderZX::funcN00_000() {
	int d = (ops > 1 ? (char)read_mem8PC() : 0);
	switch(ops) {
		// NOP
		case 0: break;
		// EX AF, AF'
		case 1: swapReg((ssh_w*)&regsZX[RA], (ssh_w*)&regsZX[RA_]); break;
		// DJNZ
		case 2: (*_B)--; if(*_B) _PC += d; break;
		// JR
		case 3: _PC += d; break;
		// JR CCC
		default: if(isFlag(ops & 3)) _PC += d;
	}
}

void DecoderZX::funcN00_010() {
	if(ops < 4) {
		// LD (BC/DE), A | LD A, (BC/DE)
		ssh_w reg = *fromRP_AF(ops);
		if(ops & 1) *_A = read_mem8(reg); else write_mem8(reg, *_A);
	} else {
		// LD [nn], HL/A | LD A/HL, [nn]
		ssh_w nn = read_mem16PC();
		ssh_w* reg = fromRP_AF(4);
		bool d = ops & 2;
		if(ops & 1) {
			if(d) *_A = read_mem8(nn); else *reg = read_mem16(nn);
		} else {
			if(d) write_mem8(nn, *_A); else write_mem16(nn, *reg);
		}
	}
}

void DecoderZX::funcN00_111() {
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
			update_flags(FS | FZ | F5 | FH | F3 | FPV | FC, (a & 128) | GET_FZ(a) | (a & 0b00101000) | fh | GET_FP(a) | fc);
			break;
		}
		// CPL; --*1*-1-
		case 5: a = ~a; update_flags(F5 | FH | F3 | FN, (a & 0b00101000) | 16 | 2); break;
		// SCF; --*0*-01
		case 6:	update_flags(F5 | FH | F3 | FN | FC, (a & 0b00101000) | 1); break;
		// CCF; --***-0C H <-старый C
		case 7: update_flags(F5 | FH | F3 | FN | FC, (a & 0b00101000) | (oldFc << 4) | (oldFc == 0)); break;
		default: a = rotate(a, F5 | FH | F3 | FN | FC);
	}
	*_A = a;
}

void DecoderZX::ldRp() {
	ssh_w* reg = fromRP_SP(ops);
	if(ops & 1) {
		// ADD HL, RP; --***-0C	F5,H,F3 берутся по результатам сложения старших байтов
		ssh_w* hl = fromRP_SP(4);
		ssh_w valHL = *hl;
		ssh_d val = valHL + *reg;
		*hl = (ssh_w)val;
		update_flags(F5 | FH | F3 | FN | FC, ((val >> 8) & 0b00101000) | calcFH(valHL >> 8, (*reg) >> 8, 0) | (val > 65535));
	} else {
		// LD RP, NN
		*reg = read_mem16PC();
	}
}

void DecoderZX::incRp() { 
	*fromRP_SP(ops) += ((ops & 1) ? -1 : 1);
}

void DecoderZX::incSSS() {
	// SZ5H3VN-
	ssh_b n = typeOps & 1;
	ssh_b* reg = fromRON(ops);
	ssh_b valReg = *reg;
	ssh_b d = (n ? -1 : 1);
	ssh_b val = valReg + d;
	write_mem8(reg, val, ops);
	update_flags(FS | FZ | F5 | FH | F3 | FPV | FN,
		(val & 0b00101000) | (val & 128) | GET_FZ(val) | calcFH(valReg, d, n) | GET_FV(valReg, val) | GET_FN(n));
}

void DecoderZX::ldSSS() {
	ssh_b* reg = fromRON(ops);
	write_mem8(reg, read_mem8PC(), ops);
}
