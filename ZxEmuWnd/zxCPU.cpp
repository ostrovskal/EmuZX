
#include "stdafx.h"
#include "zxBus.h"
#include "zxCPU.h"
#include "zxGPU.h"
#include "zxDebugger.h"

extern funcCPU table_opsN00_XXX[];
extern funcCPU table_opsN11_XXX[];
extern funcCPU table_opsED01_XXX[];
extern funcCPU table_opsED10_XXX[];

ssh_b flags_cond[4] = {FZ, FC, FPV, FS};
ssh_d cnvPrefAF[] = {RC, RB, RE, RD, RL, RH, RF, RA, RC, RB, RE, RD, RXL, RXH, RF, RA, RC, RB, RE, RD, RYL, RYH, RF, RA};
ssh_d cnvPrefSP[] = {RC, RB, RE, RD, RL, RH, RSPL, RSPH, RC, RB, RE, RD, RXL, RXH, RSPL, RSPH, RC, RB, RE, RD, RYL, RYH, RSPL, RSPH};
ssh_d cnvPrefRON[] = {RB, RC, RD, RE, RH, RL, RF, RA, RB, RC, RD, RE, RXH, RXL, RF, RA, RB, RC, RD, RE, RYH, RYL, RF, RA};
 
void zxCPU::noni() {
	modifyTSTATE(0, ZX_INT); 
}

ssh_b readPort(ssh_b lport, ssh_b hport) {
	switch(lport) {
		case 0xfd: return 0;
		case 0x1f: return *_KEMPSTON;
		case 0xfe: {
			ssh_b ret = 255;
			for(int i = 0; i < 8; i++) {
				if(hport & (1 << i)) continue;
				ret &= _KEYS[i];
			}
			return ret;
		}
	}
	return 255;
}

void writePort(ssh_b lport, ssh_b hport, ssh_b val) {
	switch(lport) {
		// covox
		case 0xfb: break;
		// specdrum
		case 0xdf: break;
		// AY
		case 0xff: break;
		// 0, 1, 2 - бордер
		// 3 MIC - при записи/загрузке
		// 4 - SOUND
		case 0xfe:
			*_PORT_FE &= 224;
			*_PORT_FE |= (val & 31);
			break;
		// 1, 2, 4 - страница 0-7
		// 8 - экран 5/7
		// 16 - ПЗУ 0 - 128К 1 - 48К
		// 32 - блокировка
		case 0xfd:
			if(hport != 0x7f) break;
			if(*_LOCK_FD) break;
			*_LOCK_FD |= val & 32;
			// ROM
			theApp->bus.swapPage(((val & 16) >> 4), zxBus::SWAP_ROM);
			// SCREEN
			theApp->bus.swapPage(((val & 8) ? 7 : 5), zxBus::SWAP_VRAM);
			// PAGE
			theApp->bus.swapPage(val & 7, zxBus::SWAP_RAM);
			break;
		case 0x1f:
			*_KEMPSTON = val;
			break;
	}
}

// пишем в память 8 битное значение
void zxCPU::write_mem8(ssh_b* address, ssh_b val) {
	if(address < &memZX[65536]) {
		if((*_TSTATE) & ZX_DEBUG) theApp->debug->checkBPS((ssh_w)(address - memZX), true);
		if(address < &memZX[16384]) return;
	}
	*address = val;
}

// пишем в память 16 битное значение
void zxCPU::write_mem16(ssh_b* address, ssh_w val) {
	if(address < &memZX[65536]) {
		if((*_TSTATE) & ZX_DEBUG) theApp->debug->checkBPS((ssh_w)(address - memZX), true);
		if(address < &memZX[16384]) return;
	}
	*(ssh_w*)address = val;
}

void zxCPU::execCALL(ssh_w address) {
	*_PC_EXIT_CALL = (*_PC);
	(*_SP) -= 2;
	write_mem16(*_SP, (*_PC));
	(*_PC) = address;
}

ssh_b zxCPU::rotate(ssh_b v, ssh_b mask) {
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

void zxCPU::opsAccum2() {
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

void zxCPU::opsAccum(ssh_b d) {
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

void zxCPU::opsNN00() {
	(this->*table_opsN00_XXX[typeOps])();
}

void zxCPU::opsNN01() {
	if(typeOps == ops && ops == 6) {
		// halt
	if(!((*_TSTATE) & ZX_TRAP)) (*_PC)--;
	} else {
		if(typeOps == 6) {
			// LD DDD, [HL/IX]
			memZX[cnvPrefRON[ops]] = *fromPrefRON(typeOps);
		} else if(ops == 6) {
			// LD [HL/IX], SSS
			write_mem8(fromPrefRON(ops), memZX[cnvPrefRON[typeOps]]);
		} else {
			// LD DDD, SSS
			write_mem8(fromPrefRON(ops), *fromPrefRON(typeOps));
		}
	}
}

void zxCPU::opsNN10() {
	opsAccum(*fromPrefRON(typeOps));
}

void zxCPU::opsNN11() {
	(this->*table_opsN11_XXX[typeOps])();
}
	
void zxCPU::opsED01() {
	(this->*table_opsED01_XXX[typeOps])();
}

void zxCPU::opsEDXX() {
	noni();
}

funcCPU table_ops[] = {
	&zxCPU::opsNN00, &zxCPU::opsCB00, &zxCPU::opsEDXX,
	&zxCPU::opsNN01, &zxCPU::opsCB01, &zxCPU::opsED01,
	&zxCPU::opsNN10, &zxCPU::opsCB10, &zxCPU::opsED10,
	&zxCPU::opsNN11, &zxCPU::opsCB11, &zxCPU::opsEDXX
};

void zxCPU::execOps(int prefix1, int prefix2) {
	incrementR();
	// читаем операцию
	ssh_b group = readOps();
	prefix = (prefix1 << 3);
	if(prefix2 == 1 && prefix) { fromPrefRON(6); group = readOps(); }
	(this->*table_ops[group * 3 + prefix2])();
}
