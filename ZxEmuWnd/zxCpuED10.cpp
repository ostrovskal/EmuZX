
#include "stdafx.h"
#include "zxCPU.h"

int zxCPU::opsED10() {
	if(ops < 4) return opsEDXX();
	ssh_w dir = ((ops & 1) ? -1 : 1);
	ssh_b a = *_A, bt, fz, d;
	ssh_b c = cpuZX[RC];
	ssh_b rep = (ops & 2);
	int tks = 16;
	if(rep) tks += (typeOps < 2 ? ((*_BC) - 1) : ((*_B) - 1)) * 21;
	bool is = false;
	switch(typeOps) {
		// LDI; --*0**0-
		// PV = 1 если после декремента BC<>0; F3 = бит 3 операции переданный байт + A; F5 = бит 1 операции переданный байт + A
		case 0:
			tks = rep ? (*_BC - 1) * 21 : 0;
			do {
				if(is) { incrementR(); incrementR(); }
				write_mem8(*_DE, read_mem8(*_HL)); (*_DE) += dir; (*_HL) += dir; (*_BC)--;
				is = ((*_BC) != 0);
			} while(rep && is);
			update_flags(FH | FPV | FN, (is << 2));
			// CPI; SZ*H**1-
			// S, Z, HC из A - [HL]
			// F3 = бит 3 операции A - [HL] - HC, где HC взят из F после предыдущей операции
			// F5 = бит 1 операции A - [HL] - HC
		case 1:
			do {
				if(is) { incrementR(); incrementR(); }
				d = read_mem8((*_HL));
				bt = a - d; fz = _FZ(bt);
				(*_HL) += dir; (*_BC)--;
				is = (*_BC) != 0;
			} while(rep && is && !fz);
			update_flags(FS | FZ | FH | FPV | FN, _FS(bt) | fz | _FH(a, d, 0, 1) | (is << 2) | 2);
			break;
			// INI [HL] <- IN[BC]; SZ5*3***
		case 2:
			do {
				if(is) { incrementR(); incrementR(); }
				write_mem8(*_HL, readPort(c, *_B));
				(*_HL) += dir; (*_B)--;
				is = ((*_B) == 0);
			} while(rep && !is);
			update_flags(FS | FZ | FH | FPV | FN, 2 | _FZ(is));
			break;
			// OUTI OUT[BC] <- [HL]; SZ5*3***
		case 3:
			do {
				if(is) { incrementR(); incrementR(); }
				writePort(c, *_B, read_mem8(*_HL));
				(*_HL) += dir; (*_B)--;
				is = ((*_B) == 0);
			} while(rep && !is);
			update_flags(FS | FZ | FH | FPV | FN, 2 | _FZ(is));
			break;
		default: opsEDXX(); return 8;
	}
	return tks;
}
