
#include "stdafx.h"
#include "DecoderZX.h"
#include "GpuZX.h"
#include "zxDebugger.h"

extern funcDecoder table_opsN00_XXX[];
extern funcDecoder table_opsN11_XXX[];
extern funcDecoder table_opsED01_XXX[];
extern funcDecoder table_opsED10_XXX[];

ssh_b flags_cond[4] = {FZ, FC, FPV, FS};
ssh_b cnvPrefAF[] = {RC, RB, RE, RD, RL, RH, RF, RA, RC, RB, RE, RD, RXL, RXH, RF, RA, RC, RB, RE, RD, RYL, RYH, RF, RA};
ssh_b cnvPrefSP[] = {RC, RB, RE, RD, RL, RH, RSPL, RSPH, RC, RB, RE, RD, RXL, RXH, RSPL, RSPH, RC, RB, RE, RD, RYL, RYH, RSPL, RSPH};
ssh_b cnvPrefRON[] = {RB, RC, RD, RE, RH, RL, RF, RA, RB, RC, RD, RE, RXH, RXL, RF, RA, RB, RC, RD, RE, RYH, RYL, RF, RA};

// пишем в память 8 битное значение
void DecoderZX::write_mem8(ssh_b* address, ssh_b val, ssh_w ron) {
	if(_TSTATE & ZX_DEBUG) debug->checkBPS((ssh_w)(address - memZX), true);
	if(address < limitROM) { if(_TSTATE & ZX_WRITE_ROM) *address = val; }
	else *address = val;
}

// пишем в память 16 битное значение
void DecoderZX::write_mem16(ssh_w address, ssh_w val) {
	ssh_b* mem = &memZX[address];
	if(_TSTATE & ZX_DEBUG) debug->checkBPS(address, true);
	if(address < 16384) { if(_TSTATE & ZX_WRITE_ROM) *(ssh_w*)mem = val; }
	else *(ssh_w*)mem = val;
}

ssh_b DecoderZX::readPort(ssh_w address) {
	if((address % 256) == 31) {
		return portsZX[31];
	}
	return portsZX[address];
}

void DecoderZX::writePort(ssh_w address, ssh_b val) {
	if(address == 0x7ffd) {
		// 1, 2, 4 - страница 0-7
		// 8 - экран 5/7
		// 16 - ПЗУ 0 - 128К 1 - 48К
		// 32 - блокировка
		*_PORT_FD &= 224;
		if(!((*_PORT_FD) & 32)) {
			*_PORT_FD |= (val & 63);
			// ROM
			int rom = ((val & 16) >> 4);
			if(rom != (*_ROM)) {
				memcpy(&memZX, ptrROM + (rom * 16384), 16384);
				*_ROM = rom;
			}
			// SCREEN
			*_VID = ((val & 8) ? 7 : 5);
			//memScreen = &memBanks[(*_VID) * 16384];
			// PAGE
			int page = val & 7;
			if(page != *_RAM) {
				memcpy(&memBanks[(*_RAM) * 16384], &memZX[0xc000], 16384);
				memcpy(&memZX[0xc000], &memBanks[page * 16384], 16384);
				*_RAM = page;
			}
		}
	} else {
		if((address % 256) == 0xfe) {
			// 0, 1, 2 - бордер
			// 3 MIC - при записи/загрузке
			// 4 - SOUND
			*_PORT_FE &= 224;
			*_PORT_FE |= (val & 31);
		} else {
			portsZX[address] = val;
		}
	}
}

void DecoderZX::execCALL(ssh_w address) {
	*_PC_EXIT_CALL = (*_PC);
	(*_SP) -= 2;
	write_mem16(*_SP, (*_PC));
	(*_PC) = address;
}

ssh_b DecoderZX::rotate(ssh_b v, ssh_b mask) {
	// --503-0C | SZ503P0C
	ssh_b fl = getFlag(FC);
	v = asm_ssh_rotate(v, ops, &fl);
	update_flags(mask, mask & fl);
	return v;
	/*
	ssh_b ofc = fl;
	ssh_b fc = ((v & ((ops & 1) ? 1 : 128)) != 0);
	ssh_b v1 = ((ops & 1) ? v >> 1 : v << 1);
	switch(ops) {
		// RLC
		case 0: v = fc; break;
		// RRC
		case 1: v = (fc << 7); break;
		// RL
		case 2: v = ofc; break;
		// RR
		case 3: v = (ofc << 7); break;
		// SLA SRL
		case 4: case 7: v = 0; break;
		// SRA
		case 5: v &= 128; break;
		// SLI
		case 6: v = 1; break;
	}
	v |= v1;
	update_flags(mask, mask & ( _FS(v) | _FZ(v) | _FP(v) | fc));
	return v;
	*/
}

void DecoderZX::opsAccum2() {
	ssh_b fl = getFlag(FC);
	*_HL = asm_ssh_accum2(*_HL, *fromRP_SP(ops), (!(ops & 1)) << 1, &fl);
	update_flags(FS | FZ | FH | FPV | FN | FC, fl);
	/*
	ssh_w op1 = *_HL;
	ssh_w op2 = *fromRP_SP(ops);
	ssh_d val = (fn ? op1 - (op2 + fl) : op1 + (op2 + fl));
	ssh_w res = (ssh_w)val;
	ssh_b fc = val > 65535;
	ssh_b fz = (res == 0) << 6;
	ssh_b fh = _FH(op1 >> 8, op2 >> 8, fl, fn);
	ssh_b fv = _FV2(op1, op2, fl, fn);
	ssh_b fs = _FS(res >> 8);
	*_HL = (ssh_w)res;
	ssh_w res1 = asm_ssh_accum2(op1, op2, fn, &fl);
	ssh_b flags = fs | fz | fh | fv | fn | fc;
	if(res != res1 || flags != fl) {
		fl = getFlag(FC);
		fh = _FH(op1 >> 8, op2 >> 8, fl, fn);
		fv = _FV2(op1, op2, fl, fn);
		res1 = asm_ssh_accum2(op1, op2, fn, &fl);
	}
	update_flags(FS | FZ | FH | FPV | FN | FC, flags);
	*/
}

void DecoderZX::opsAccum(ssh_b d) {
	ssh_b fl = getFlag(FC);
	ssh_b ret = asm_ssh_accum(*_A, d, ops, &fl);
	if(ops != 7) *_A = ret;
	update_flags(FS | FZ | FH | FPV | FN | FC, fl);
	/*
	ssh_b a = *_A;
	ssh_b op1 = a;
	ssh_b op2 = d;
	ssh_w res = 0;

	ssh_b fl = getFlag(FC);
	ssh_b fn = 0;
	ssh_b fh = 255;
	ssh_b fc = 0;
	
	switch(ops) {
		// ADD A, d; SZ5H3V0C
		case 0: res = op1 + op2; break;
		// ADC A, d; SZ5H3V0C
		case 1: res = op1 + op2 + fl; fc = fl; break;
		// SUB A, d/CP A, d; SZ5H3V1C
		case 2: case 7: res = op1 - op2; fn = 2; break;
		// SBC A, d; SZ5H3V1C
		case 3: res = op1 - (op2 + fl); fc = fl; fn = 2; break;
		// AND A, d; SZ513P00
		case 4: res = op1 & op2; fh = 16; break;
		// XOR A, d; SZ503P00
		case 5: res = op1 ^ op2; fh = 0; break;
		// OR A, d; SZ503P00
		case 6: res = op1 | op2; fh = 0; break;
	}
	ssh_b val = (ssh_b)res;
	ssh_b fpv;
	if(fh == 255) fh = _FH(op1, op2, fc, fn);
	if(ops < 4 || ops >= 7) {
		fpv = _FV1(op1, op2, fc, fn);
		fc = res > 255;
	}
	else {
		fpv = _FP(val);
		fc = 0;
	}
	if(ops != 7) *_A = val;
	ssh_b flags = _FS(val) | _FZ(val) | fh | fpv | fn | fc;

	ssh_b res1 = asm_ssh_accum(op1, d, ops, &fl);

	//fl |= fh;
	if(fl != flags) {
		fl = getFlag(FC);
		fh = _FH(op1, op2, fl, fn);;
		fpv = calcFV1(op1, op2, fl, fn);
		asm_ssh_accum(op1, op2, ops, &fl);
	}
	update_flags(FS | FZ | FH | FPV | FN | FC, flags);
	*/
}

void DecoderZX::ops00_NO() {
	(this->*table_opsN00_XXX[typeOps])();
}

void DecoderZX::ops01_NO() {
	if(typeOps == ops && ops == 6) {
		// halt
	if(!(_TSTATE & ZX_TRAP)) (*_PC)--;
	} else {
		if(typeOps == 6) {
			// LD DDD, [HL/IX]
			regsZX[cnvPrefRON[ops]] = *fromRON(typeOps);
		} else if(ops == 6) {
			// LD [HL/IX], SSS
			write_mem8(fromRON(ops), regsZX[cnvPrefRON[typeOps]], ops);
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
	prefix = (prefix1 << 3);
	if(prefix2 == 1 && prefix) { fromRON(6); group = readOps(); }
	(this->*table_ops[group * 3 + prefix2])();
}
