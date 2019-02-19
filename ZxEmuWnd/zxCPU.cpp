
#include "stdafx.h"
#include "zxBus.h"
#include "zxCPU.h"
#include "zxGPU.h"
#include "zxDebugger.h"

extern funcCPU table_opsN00_XXX[];
extern funcCPU table_opsN11_XXX[];
extern funcCPU table_opsED01_XXX[];
extern funcCPU table_opsED10_XXX[];

int ticks;

ssh_b flags_cond[4] = {FZ, FC, FPV, FS};
ssh_b tbl_hcarry[16] = {0, 0, 1, 0, 1, 0, 1, 1,		0, 1, 1, 1, 0, 0, 0, 1};
ssh_b tbl_overflow[16] = {0, 1, 0, 0, 0, 0, 1, 0,	0, 0, 0, 1, 1, 0, 0, 0};
ssh_b tbl_parity[256];

ssh_b cnvPrefAF[] = {RC, RB, RE, RD, RL, RH, RF, RA, RC, RB, RE, RD, RXL, RXH, RF, RA, RC, RB, RE, RD, RYL, RYH, RF, RA};
ssh_b cnvPrefSP[] = {RC, RB, RE, RD, RL, RH, RSPL, RSPH, RC, RB, RE, RD, RXL, RXH, RSPL, RSPH, RC, RB, RE, RD, RYL, RYH, RSPL, RSPH};
ssh_b cnvPrefRON[] = {RB, RC, RD, RE, RH, RL, RF, RA, RB, RC, RD, RE, RXH, RXL, RF, RA, RB, RC, RD, RE, RYH, RYL, RF, RA};

void makeTblParity() {
	for(int a = 0; a < 256; a++) {
		ssh_b val = a;
		val = ((a >> 1) & 0x55) + (val & 0x55);
		val = ((val >> 2) & 0x33) + (val & 0x33);
		val = ((val >> 4) & 0x0F) + (val & 0x0F);
		tbl_parity[a] = val;
	}
}

// пишем в память 8 битное значение
void zxCPU::write_mem8(ssh_w address, ssh_b val) {
	if((*_TSTATE) & ZX_DEBUG) theApp->debug->checkBPS(address, true);
	write_mem8(get_mem(address), val);
}

// пишем в память 8 битное значение
void zxCPU::write_mem16(ssh_w address, ssh_w val) {
	if((*_TSTATE) & ZX_DEBUG) theApp->debug->checkBPS(address, true);
	write_mem8(address, val & 0xff);
	write_mem8(address + 1, val >> 8);
}

int zxCPU::noni() {
	modifyTSTATE(0, ZX_INT); 
	return 4;
}

int zxCPU::execCALL(ssh_w address) {
	*_PC_EXIT_CALL = (*_PC);
	(*_SP) -= 2;
	write_mem16(*_SP, (*_PC));
	(*_PC) = address;
	return 17;
}

ssh_b zxCPU::rotate(ssh_b v, ssh_b mask) {
	// --503-0C | SZ503P0C
	ssh_b fl = getFlag(FC);
	v = asm_ssh_rotate(v, ops, &fl);
	update_flags(mask, mask & fl);
	return v;
}

int zxCPU::opsAccum2() {
	ssh_b fl = getFlag(FC);
	*_HL = asm_ssh_accum2(*_HL, *fromRP_SP(ops), (!(ops & 1)) << 1, &fl);
	update_flags(FS | FZ | FH | FPV | FN | FC, fl);
	return 15;
}

// 4/7/15
int zxCPU::opsAccum(ssh_b d) {
	ssh_b fl = getFlag(FC);
	ssh_b ret = asm_ssh_accum(*_A, d, ops, &fl);
	if(ops != 7) *_A = ret;
	update_flags(FS | FZ | FH | FPV | FN | FC, fl);
	return 7;
}

int zxCPU::opsNN00() {
	return (this->*table_opsN00_XXX[typeOps])();
}

int zxCPU::opsNN01() {
	if(typeOps == ops && ops == 6) {
		// halt
		modifyTSTATE(ZX_HALT, 0);
		return 4;
	}
	if(typeOps == 6) {
		// LD DDD, [HL/IX]
		cpuZX[cnvPrefRON[ops]] = *fromPrefRON(typeOps);
	} else if(ops == 6) {
		// LD [HL/IX], SSS
		write_mem8(fromPrefRON(ops), cpuZX[cnvPrefRON[typeOps]]);
	} else {
		// LD DDD, SSS
		write_mem8(fromPrefRON(ops), *fromPrefRON(typeOps));
		return 4;
	}
	return (prefix ? 15 : 7);
}

int zxCPU::opsNN10() {
	opsAccum(*fromPrefRON(typeOps));
	return (prefix ? 15 : (typeOps == 6 ? 7 : 4));
}

int zxCPU::opsNN11() {
	return (this->*table_opsN11_XXX[typeOps])();
}
	
int zxCPU::opsED01() {
	return (this->*table_opsED01_XXX[typeOps])();
}

int zxCPU::opsEDXX() {
	return noni();
}

funcCPU table_ops[] = {
	&zxCPU::opsNN00, &zxCPU::opsCB00, &zxCPU::opsEDXX,
	&zxCPU::opsNN01, &zxCPU::opsCB01, &zxCPU::opsED01,
	&zxCPU::opsNN10, &zxCPU::opsCB10, &zxCPU::opsED10,
	&zxCPU::opsNN11, &zxCPU::opsCB11, &zxCPU::opsEDXX
};

int zxCPU::execOps(int prefix1, int prefix2) {
	if(!(*_TSTATE & ZX_HALT)) {
		incrementR();
		// читаем операцию
		ssh_b group = readOps();
		prefix = (prefix1 << 3);
		if(prefix) ticks += 4;
		if(prefix2 == 1 && prefix) { fromPrefRON(6); group = readOps(); }
		ticks += (this->*table_ops[group * 3 + prefix2])();
	} else ticks += 4;
	return ticks;
}
