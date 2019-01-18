#include "stdafx.h"
#include "DecoderZX.h"

funcOps table_opsN00_XXX[] = {
	&DecoderZX::funcN00_000, &DecoderZX::ldRp, &DecoderZX::funcN00_010, &DecoderZX::incRp, &DecoderZX::incSSS, &DecoderZX::incSSS, &DecoderZX::ldSSS, &DecoderZX::funcN00_111
};

void DecoderZX::funcN00_000() {
	int d = (ops > 1 ? (char)read_mem8PC() : 0);
	switch(ops) {
		// NOP
		case 0: break;
			// EX AF, AF'
		case 1: swapReg((word*)&CpuZX::cpu[RA], (word*)&CpuZX::cpu[RA + 14]); break;
			// DJNZ
		case 2: (*CpuZX::B)--; if(*CpuZX::B) CpuZX::RPC += d; break;
			// JR
		case 3: CpuZX::RPC += d; break;
			// JR CCC
		default: if(isFlag(ops & 3)) CpuZX::RPC += d;
	}
}

void DecoderZX::funcN00_010() {
	if(ops < 4) {
		// LD (BC/DE), A | LD A, (BC/DE)
		word reg = *fromRP_AF(ops);
		if(ops & 1) *CpuZX::A = read_mem8(reg); else write_mem8(reg, *CpuZX::A);
	} else {
		// LD [nn], HL/A | LD A/HL, [nn]
		word nn = read_mem16PC();
		word* reg = fromRP_AF(4);
		bool d = ops & 2;
		if(ops & 1) {
			if(d) *CpuZX::A = read_mem8(nn); else *reg = read_mem16(nn);
		} else {
			if(d) write_mem8(nn, *CpuZX::A); else write_mem16(nn, *reg);
		}
	}
}

void DecoderZX::funcN00_111() {
	byte a = *CpuZX::A;
	byte oldFc = getFlag(FC);
	switch(ops) {
		// DAA SZ5*3P-*
		case 4: {
			byte oldA = a;
			byte fh = getFlag(FH);
			byte fn = getFlag(FN);
			byte fc = 0;
			if(((a & 15) > 9) || fh) {
				word val = a + (fn ? -6 : 6);
				a = (byte)val;
				fc = oldFc | (val > 255 ? 1 : 0);
				fh = 16;
			} else fh = 0;
			if((oldA > 0x99) || oldFc) {
				a += (fn ? -96 : 96);
				fc = 1;
			} else fc = 0;
			update_flags(FS | FZ | F5 | FH | F3 | FPV | FC,
						 (a & 128) | GET_FZ(a) | (a & 0b00101000) | fh | GET_FP(a) | fc);
			break;
		}
		// CPL; A <- NOT A --*1*-1-
		case 5: a = ~a; update_flags(F5 | FH | F3 | FN, (a & 0b00101000) | 16 | 2); break;
		// SCF; CY = 1 --*0*-01
		case 6:	update_flags(F5 | FH | F3 | FN | FC, (a & 0b00101000) | 1); break;
		// CCF; CY <-NOT CY --***-0C H <-старый C
		case 7: update_flags(F5 | FH | F3 | FN | FC, (a & 0b00101000) | (oldFc << 4) | (oldFc == 0)); break;
		default: a = rotate(a, F5 | FH | F3 | FN | FC);
	}
	*CpuZX::A = a;
}

void DecoderZX::ldRp() {
	word* reg = fromRP_SP(ops);
	if(ops & 1) {
		// ADD HL, RP; --***-0C	F5,H,F3 берутся по результатам сложения старших байтов
		word* hl = fromRP_SP(4);
		word valHL = *hl;
		dword val = valHL + *reg;
		*hl = (word)val;
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
	byte n = typeOps & 1;
	byte* reg = fromRON(ops);
	byte valReg = *reg;
	byte d = (n ? -1 : 1);
	byte val = valReg + d;
	write_mem8(reg, val, ops);
	update_flags(FS | FZ | F5 | FH | F3 | FPV | FN,
		(val & 0b00101000) | (val & 128) | GET_FZ(val) | calcFH(valReg, d, n) | GET_FV(valReg, val) | GET_FN(n));
}

void DecoderZX::ldSSS() {
	byte* reg = fromRON(ops);
	write_mem8(reg, read_mem8PC(), ops);
}
