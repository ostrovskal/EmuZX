
#include "stdafx.h"
#include "DecoderZX.h"

void DecoderZX::ops10_ED() {
	if(ops < 4) noni();
	else {
		ssh_w d = ((ops & 1) ? -1 : 1);
		ssh_w hl = *CpuZX::HL;
		ssh_b a = *CpuZX::A;
		ssh_b rep = (ops & 2);
		switch(typeOps) {
			// LDI [DE] <- [HL]; DE += d; HL += d; BC--; --*0**0-
			// PV = 1 если после декремента BC<>0; F3 = бит 3 операции переданный байт + A; F5 = бит 1 операции переданный байт + A
			case 0: {
				ssh_b bt = CpuZX::memory[hl];
				write_mem8(*CpuZX::DE, bt); (*CpuZX::DE) += d; (*CpuZX::HL) += d; (*CpuZX::BC)--;
				bool bc = *CpuZX::BC != 0;
				ssh_b f5 = ((bt + a) & 0b00001000) << 2;
				ssh_b f3 = ((bt + a) & 1) << 3;
				update_flags(F5 | FH | F3 | FPV | FN, (bc << 2) | f5 | f3);
				if(rep && bc) CpuZX::PC -= 2;
				break;
			}
			// CPI A - [HL]; HL += d; BC--; SZ*H**1-
			// S, Z, HC из A - [HL]
			// F3 = бит 3 операции A - [HL] - HC, где HC взят из F после предыдущей операции
			// F5 = бит 1 операции A - [HL] - HC
			case 1: {
				ssh_b bt = a - CpuZX::memory[hl];
				ssh_b fz = GET_FZ(bt);
				(*CpuZX::HL) += d; (*CpuZX::BC)--;
				bool bc = *CpuZX::BC != 0;
				update_flags(FS | FZ | F5 | FH | F3 | FPV | FN, (bt & 128) | fz | (bc << 2) | 2);
				if(rep && bc && !fz) CpuZX::PC -= 2;
				break;
			}
			// INI [HL] <- IN[BC]; HL += d; B--; SZ5*3***
			case 2: {
				write_mem8(hl, CpuZX::port[*CpuZX::BC]);
				(*CpuZX::HL) += ((ops & 1) ? -1 : 1); (*CpuZX::B)--;
				if(rep && *CpuZX::B) CpuZX::PC -= 2;
				break;
			}
			// OUTI OUT[BC] <- [HL]; HL += d; B--; SZ5*3***
			case 3: {
				CpuZX::port[*CpuZX::BC] = CpuZX::memory[hl];
				(*CpuZX::HL) += ((ops & 1) ? -1 : 1); (*CpuZX::B)--;
				if(rep && *CpuZX::B) CpuZX::PC -= 2;
				break;
			}
			default: noni();
		}
	}
}
