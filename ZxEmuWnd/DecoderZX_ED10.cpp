
#include "stdafx.h"
#include "DecoderZX.h"

void DecoderZX::ops10_ED() {
	if(ops < 4) opsXX_ED();
	else {
		ssh_w d = ((ops & 1) ? -1 : 1);
		ssh_w hl = *_HL;
		ssh_w bc = *_BC;
		ssh_b a = *_A;
		ssh_b rep = (ops & 2);
		bool is;
		switch(typeOps) {
			// LDI; --*0**0-
			// PV = 1 если после декремента BC<>0; F3 = бит 3 операции переданный байт + A; F5 = бит 1 операции переданный байт + A
			case 0: {
				write_mem8(*_DE, read_mem8(hl)); (*_DE) += d; (*_HL) += d; (*_BC)--;
				is = ((*_BC) != 0);
				update_flags(FH | FPV | FN, (is << 2));
				if(rep && is) _PC -= 2;
				break;
			}
			// CPI; SZ*H**1-
			// S, Z, HC из A - [HL]
			// F3 = бит 3 операции A - [HL] - HC, где HC взят из F после предыдущей операции
			// F5 = бит 1 операции A - [HL] - HC
			case 1: {
				ssh_b d = read_mem8(hl);
				ssh_b bt = a - d;
				ssh_b fz = GET_FZ(bt);
				(*_HL) += d; (*_BC)--;
				is = *_BC != 0;
				if(!is || !rep) {
					update_flags(FS | FZ | F5 | FH | F3 | FPV | FN, (bt & 128) | fz | calcFH(a, d, 1) | (is << 2) | 2);
				}
				if(rep && is && !fz) _PC -= 2;
				break;
			}
			// INI [HL] <- IN[BC]; HL += d; B--; SZ5*3***
			case 2: {
				write_mem8(hl, portsZX[bc]);
				(*_HL) += d; (*_B)--;
				is = *_B == 0;
				if(is || !rep) {
					update_flags(FS | FZ | FH | FPV | FN, 2 | (is << 6));
				}
				if(rep && !is) _PC -= 2;
				break;
			}
			// OUTI OUT[BC] <- [HL]; HL += d; B--; SZ5*3***
			case 3: {
				portsZX[*_BC] = read_mem8(hl);
				(*_HL) += d; (*_B)--;
				is = *_B == 0;
				if(is || !rep) {
					update_flags(FS | FZ | FH | FPV | FN, 2 | (is << 6));
				}
				if(rep && !is) _PC -= 2;
				break;
			}
			default: opsXX_ED();
		}
	}
}
