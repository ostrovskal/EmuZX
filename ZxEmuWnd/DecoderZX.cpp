
#include "stdafx.h"
#include "DecoderZX.h"

extern funcOps table_opsN00_XXX[];
extern funcOps table_opsN11_XXX[];
extern funcOps table_opsED01_XXX[];
extern funcOps table_opsED10_XXX[];

byte flags_cond[4] = {FZ, FC, FPV, FS};
byte cnvPrefixAF[] = {RC, RB, RE, RD, RL, RH, RA, RF, RC, RB, RE, RD, RIXL, RIXH, RA, RF, RC, RB, RE, RD, RIYL, RIYH, RA, RF};
byte cnvPrefixSP[] = {RC, RB, RE, RD, RL, RH, RSPL, RSPH, RC, RB, RE, RD, RIXL, RIXH, RSPL, RSPH, RC, RB, RE, RD, RIYL, RIYH, RSPL, RSPH};

// вычисление четности
byte calcFP(byte val) {
	val = ((val >> 1) & 0x55) + (val & 0x55);
	val = ((val >> 2) & 0x33) + (val & 0x33);
	val = ((val >> 4) & 0x0F) + (val & 0x0F);
	return !(val & 1);
}

// вычисление переполнения
byte calcFV(byte val1, byte val2) {
	if((val1 & 128) != (val2 & 128)) return 4;
	return 0;
}

// вычисление полупереноса
byte calcFH(byte val, byte offs, byte fn) {
	byte v = val & 15;
	byte o = offs & 15;
	byte ret = fn ? v - o : v + o;
	return (ret > 15) << 4;
}

void DecoderZX::swapReg(word* r1, word* r2) {
	word a = *r1;
	word b = *r2;
	*r2 = a;
	*r1 = b;
}

void DecoderZX::execCALL(word address) {
	(*CpuZX::SP) -= 2;
	write_mem16(*CpuZX::SP, CpuZX::RPC);
	CpuZX::RPC = address;
}

byte DecoderZX::rotate(byte v, int msk) {
	// --503-0C | SZ503P0C
	byte fc = (v & ((ops & 1) ? 1 : 128));
	byte v1 = ((ops & 1) ? v >> 1 : v << 1);
	switch(ops) {
		// RLC
		case 0: v = (fc == 128); break;
		// RRC
		case 1: v = ((fc == 1) << 7); break;
		// RL
		case 2: v = getFlag(FC); break;
		// RR
		case 3: v = (getFlag(FC) << 7); break;
		// SLA SRL
		case 4: case 7: v = 0; break;
		// SRA
		case 5: v &= 128; break;
		// SLI
		case 6: v = 1; break;
	}
	v |= v1;
	update_flags(msk, msk & ((v & 128) | GET_FZ(v) | (v & 0b00101000) | GET_FP(v) | (fc != 0)));
	return v;
}

void DecoderZX::opsAccum(byte d) {
	word nVal = 0;
	byte a = *CpuZX::A;
	byte fn = 0;
	byte fh = 255;
	byte fc = getFlag(FC);

	switch(ops) {
		// ADD A, d; A <- A + d; SZ5H3V0C
		case 0: nVal = a + d; break;
		// ADC A, d; A <- A + d + CY; SZ5H3V0C
		case 1: nVal = a + (d + fc); break;
		// SUB A, d; A <- A - d; SZ5H3V1C
		case 2: nVal = a - d; fn = 2; break;
		// SBC A, d; A <- A - d - CY; SZ5H3V1C
		case 3: nVal = a - (d + fc); fn = 2; break;
		// AND A, d; A <- A AND d; SZ513P00
		case 4: nVal = a & d; fh = 16; break;
		// XOR A, d; A <- A XOR d; SZ503P00
		case 5: nVal = a ^ d; fh = 0; break;
		// OR A, d; A <- A OR d; SZ503P00
		case 6: nVal = a | d; fh = 0; break;
		// CP A, d; A - d; SZ*H*V1C; F5 и F3 - копия d
		case 7: nVal = a - d; fn = 2; break;
	}
	byte val = (byte)nVal;
	byte fpv;
	if(fh == 255) fh = calcFH(a, d, fn);
	if(ops < 4 || ops >= 7) {
		fc = nVal > 255;
		fpv = GET_FV(a, val);
	}
	else {
		fc = 0;
		fpv = GET_FP(val); 
	}
	if(ops != 7) *CpuZX::A = val;

	update_flags(FS | FZ | F5 | FH | F3 | FPV | FN | FC, ((ops == 7 ? d : val) & 0b00101000) | (val & 128) | GET_FZ(val) | fh | fpv | fn | fc);
}

void DecoderZX::ops00_NO() {
	(this->*table_opsN00_XXX[typeOps])();
}

void DecoderZX::ops01_NO() {
	if(typeOps == ops && ops == 6) {
		// halt
	if(!CpuZX::trap) CpuZX::RPC--;
	} else {
		if(typeOps == 6) {
			// LD DDD, [HL/IX]
			CpuZX::cpu[(RC + ((ops & 7) ^ 1))] = *fromRON(typeOps);
		} else if(ops == 6) {
			// LD [HL/IX], SSS
			write_mem8(fromRON(ops), CpuZX::cpu[(RC + ((typeOps & 7) ^ 1))], ops);
		} else {
			// LD DDD, SSS
			write_mem8(fromRON(ops), *fromRON(typeOps), ops);
		}
	}
}

void DecoderZX::ops10_NO() {
	opsAccum(*fromRON(typeOps));
}

void DecoderZX::ops11_NO() {
	(this->*table_opsN11_XXX[typeOps])();
}
	
void DecoderZX::opsXX_ED() {
	noni();
}

void DecoderZX::ops01_ED() {
	(this->*table_opsED01_XXX[typeOps])();
}

funcOps table_ops[] = {
	&DecoderZX::ops00_NO, &DecoderZX::ops00_CB, &DecoderZX::opsXX_ED,
	&DecoderZX::ops01_NO, &DecoderZX::ops01_CB, &DecoderZX::ops01_ED,
	&DecoderZX::ops10_NO, &DecoderZX::ops10_CB, &DecoderZX::ops10_ED,
	&DecoderZX::ops11_NO, &DecoderZX::ops11_CB, &DecoderZX::opsXX_ED
};

void DecoderZX::execOps(int prefix1, int prefix2) {
	// инкремент R
	byte r1 = CpuZX::RR & 127;
	CpuZX::RR = (CpuZX::RR & 128) | (++r1 & 127);
	// читаем операцию
	byte group = readOps();
	prefix = prefix1;
	if(prefix2 == 1 && prefix) { fromRON(6); group = readOps(); }
	(this->*table_ops[group * 3 + prefix2])();
}