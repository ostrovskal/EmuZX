
#include "stdafx.h"
#include "zxCPU.h"

// PV <- IFF2; SZ503*0-; Известная ошибка : если в момент выполнения команды получено прерывание, то в PV вместо 1 ошибочно попадает 0. Есть программы, которые строят на этом защиту от эмуляции.

funcCPU table_opsED01_XXX[] = {
	&zxCPU::funcED01_000, &zxCPU::funcED01_000, &zxCPU::funcED01_000, &zxCPU::funcED01_000, &zxCPU::funcED01_000,
	&zxCPU::funcED01_000, &zxCPU::funcED01_000, &zxCPU::funcED01_111
};

int zxCPU::funcED01_000() {
	ssh_b val, a;
	switch(typeOps) {
		// *IN F, [C]; SZ503P0-
		// IN DDD, [C]; SZ503P0-
		case 0:
			val = theApp->bus.readPort(cpuZX[RC], *_B);
			if(ops != 6) *fromPrefRON(ops) = val;
			update_flags(FS | FZ |FH | FPV | FN, _FS(val) | _FZ(val) | _FP(val));
			return 12;
		//	*OUT[C], 0
		//	OUT[C], SSS
		case 1: theApp->bus.writePort(cpuZX[RC], *_B, (ops == 6 ? 0 : *fromPrefRON(ops))); return 12;
		// ADC/SBC HL, RP; SZ***VNC
		case 2: return opsAccum2();
		// LD [NN], RP / LD RP, [nn]
		case 3: {
			ssh_w* reg = fromRP_SP(ops);
			ssh_w nn = read_mem16PC();
			if(ops & 1) *reg = read_mem16(nn); else write_mem16(nn, *reg);
			return 20;
		}
		// NEG; A <- 0 - A; SZ5H3V1C; PV=1 если перед операцией A=80; С=1 если перед операцией A <> 0
		case 4:
			a = *_A;
			val = -a;
			*_A = val;
			update_flags(FS | FZ | FH | FPV | FN | FC, _FS(val) | _FZ(val) | _FH(0, a, 0, 1) | ((a >= 0x80) << 2) | 2 | (a != 0));
			return 8;
		// RETI/RETN; IFF1 <- IFF2; SP += 2; PC <- [SP - 2];	Возврат из INT
		case 5:
			*_IFF1 = *_IFF2;
			(*_PC) = read_mem16(*_SP);
			(*_SP) += 2;
			return 14;
		// IM X
		case 6: 
			switch(ops) {
				case 2: case 6: *_IM = 1; break;
				case 3: case 7: *_IM = 2; break;
				default: *_IM = 0;
			}
	}
	return 8;
}

int zxCPU::funcED01_111() {
	switch(ops) {
		// LD I, A
		case 0: *_I = *_A; return 9;
		// LD R, A
		case 1: *_R = *_A; return 9;
		// LD A, I
		case 2: flagsIR(*_A = *_I); return 9;
		// LD A, R; PV <- IFF2 SZ503*0-
		case 3: flagsIR(*_A = *_R); return 9;
		// RRD/RLD; SZ503P0-
		case 4: case 5: {
			ssh_b* reg = fromRON(6);
			ssh_b vreg = *reg;
			ssh_b a = *_A & 15;
			ssh_b h;
			if(ops == 4) {
				h = vreg & 15;
				*reg = (vreg >> 4) | (a << 4);
			} else {
				h = (vreg & 240) >> 4;
				*reg = (vreg << 4) | a;
			}
			ssh_b val = *_A = ((*_A & 240) | h);
			update_flags(FS | FZ | FH | FPV | FN, _FS(val) | _FZ(val) | _FP(val));
			return 18;
		}
	}
	return 8;
}

