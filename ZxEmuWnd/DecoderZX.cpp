
#include "stdafx.h"
#include "DecoderZX.h"
#include "GpuZX.h"
#include "zxDebugger.h"

extern funcDecoder table_opsN00_XXX[];
extern funcDecoder table_opsN11_XXX[];
extern funcDecoder table_opsED01_XXX[];
extern funcDecoder table_opsED10_XXX[];

ssh_b flags_cond[4] = {FZ, FC, FPV, FS};
ssh_b cnvPrefixAF[] = {RC, RB, RE, RD, RL, RH, RA, RF, RC, RB, RE, RD, RIXL, RIXH, RA, RF, RC, RB, RE, RD, RIYL, RIYH, RA, RF};
ssh_b cnvPrefixSP[] = {RC, RB, RE, RD, RL, RH, RSPL, RSPH, RC, RB, RE, RD, RIXL, RIXH, RSPL, RSPH, RC, RB, RE, RD, RIYL, RIYH, RSPL, RSPH};

// пишем в память 8 битное значение
void DecoderZX::write_mem8(ssh_b* address, ssh_b val, ssh_w ron) {
	if((_TSTATE & ZX_DEBUG) == ZX_DEBUG) debug->checkBPS((ssh_w)(address - memZX), true);
	if(address < &memZX[16384] && (_TSTATE & ZX_WRITE_ROM) == ZX_WRITE_ROM) { *address = val; }
	else if(address < &memZX[23296] && (_TSTATE & ZX_WRITE_GPU) == ZX_WRITE_GPU) gpu->write(address, val);
	else *address = val;
}

// пишем в память 16 битное значение
void DecoderZX::write_mem16(ssh_w address, ssh_w val) {
	ssh_b* mem = &memZX[address];
	if((_TSTATE & ZX_DEBUG) == ZX_DEBUG) debug->checkBPS(address, true);
	if(address < 16384 && (_TSTATE & ZX_WRITE_ROM) == ZX_WRITE_ROM) { *(ssh_w*)mem = val; }
	else if(address < 23296 && (_TSTATE & ZX_WRITE_GPU) == ZX_WRITE_GPU) {
		gpu->write(mem, (ssh_b)val);
		gpu->write(mem + 1, val >> 8);
	} else *(ssh_w*)mem = val;
}

ssh_b DecoderZX::readPort(ssh_w address) {
	if((address % 256) == 31) {
		return portsZX[31];
	}
	return portsZX[address];
}

void DecoderZX::writePort(ssh_w address, ssh_b val) {
	if((address % 256 ) == 254) {
		// 0, 1, 2 - бордер
		// 3 MIC - при записи/загрузке
		// 4 - SOUND
		*_PORT_FE &= 224;
		*_PORT_FE |= (val & 31);
		modifyTSTATE(ZX_SOUND | ZX_BORDER, 0);
		return;
	}
	portsZX[address] = val;

}

void DecoderZX::execCALL(ssh_w address) {
	(*_SP) -= 2;
	write_mem16(*_SP, _PC);
	_PC = address;
}

ssh_b DecoderZX::rotate(ssh_b v, bool ed) {
	// --503-0C | SZ503P0C
	ssh_b ofc = getFlag(FC);
	ssh_b fc = ((v & ((ops & 1) ? 1 : 128)) != 0);
	ssh_b v1 = ((ops & 1) ? v >> 1 : v << 1);
	switch(ops) {
		// RLC
		case 0: v = fc; break;
		// RRC
		case 1: v = fc << 7; break;
		// RL
		case 2: v = ofc; break;
		// RR
		case 3: v = ofc << 7; break;
		// SLA SRL
		case 4: case 7: v = 0; break;
		// SRA
		case 5: v &= 128; break;
		// SLI
		case 6: v = 1; break;
	}
	v |= v1;
	if(ed) {
		update_flags(FS | FZ | F5 | FH | F3 | FPV | FN | FC, (v & 128) | GET_FZ(v) | (v & 0b00101000) | GET_FP(v) | fc);
	} else {
		update_flags(F5 | FH | F3 | FN | FC, (v & 0b00101000) | fc);
	}
	return v;
}

void DecoderZX::opsAccum(ssh_b d) {
	ssh_w nVal = 0;
	ssh_b a = *_A;
	ssh_b fn = 0;
	ssh_b fh = 255;
	ssh_b fc = getFlag(FC);

	switch(ops) {
		// ADD A, d; SZ5H3V0C
		case 0: nVal = a + d; break;
		// ADC A, d; SZ5H3V0C
		case 1: nVal = a + (d + fc); break;
		// SUB A, d; SZ5H3V1C
		case 2: nVal = a - d; fn = 2; break;
		// SBC A, d; SZ5H3V1C
		case 3: nVal = a - (d + fc); fn = 2; break;
		// AND A, d; SZ513P00
		case 4: nVal = a & d; fh = 16; break;
		// XOR A, d; SZ503P00
		case 5: nVal = a ^ d; fh = 0; break;
		// OR A, d; SZ503P00
		case 6: nVal = a | d; fh = 0; break;
		// CP A, d; SZ*H*V1C; F5 и F3 - копия d
		case 7: nVal = a - d; fn = 2; break;
	}
	ssh_b val = (ssh_b)nVal;
	ssh_b fpv;
	if(fh == 255) fh = calcFH(a, d, fn);
	if(ops < 4 || ops >= 7) {
		fc = nVal > 255;
		fpv = GET_FV(a, val);
	}
	else {
		fc = 0;
		fpv = GET_FP(val); 
	}
	if(ops != 7) *_A = val;

	update_flags(FS | FZ | F5 | FH | F3 | FPV | FN | FC, ((ops == 7 ? d : val) & 0b00101000) | (val & 128) | GET_FZ(val) | fh | fpv | fn | fc);
}

void DecoderZX::ops00_NO() {
	(this->*table_opsN00_XXX[typeOps])();
}

void DecoderZX::ops01_NO() {
	if(typeOps == ops && ops == 6) {
		// halt
	if(!_TRAP) _PC--;
	} else {
		if(typeOps == 6) {
			// LD DDD, [HL/IX]
			regsZX[(RC + ((ops & 7) ^ 1))] = *fromRON(typeOps);
		} else if(ops == 6) {
			// LD [HL/IX], SSS
			write_mem8(fromRON(ops), regsZX[(RC + ((typeOps & 7) ^ 1))], ops);
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
	
void DecoderZX::ops01_ED() {
	(this->*table_opsED01_XXX[typeOps])();
}

void DecoderZX::opsXX_ED() {
	_PC--;
	noni();
}

funcDecoder table_ops[] = {
	&DecoderZX::ops00_NO, &DecoderZX::ops00_CB, &DecoderZX::opsXX_ED,
	&DecoderZX::ops01_NO, &DecoderZX::ops01_CB, &DecoderZX::ops01_ED,
	&DecoderZX::ops10_NO, &DecoderZX::ops10_CB, &DecoderZX::ops10_ED,
	&DecoderZX::ops11_NO, &DecoderZX::ops11_CB, &DecoderZX::opsXX_ED
};

void DecoderZX::execOps(int prefix1, int prefix2) {
	incrementR();
	// читаем операцию
	ssh_b group = readOps();
	prefix = prefix1;
	if(prefix2 == 1 && prefix) { fromRON(6); group = readOps(); }
	(this->*table_ops[group * 3 + prefix2])();
}
