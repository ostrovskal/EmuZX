
#include "stdafx.h"
#include "DecoderZX.h"

void DecoderZX::ops10_ED() {
	if(ops < 4) noni();
	else {
		word d = ((ops & 1) ? -1 : 1);
		word hl = *CpuZX::HL;
		byte a = *CpuZX::A;
		byte rep = (ops & 2);
		switch(typeOps) {
			// LDI [DE] <- [HL]; DE += d; HL += d; BC--; --*0**0-
			// PV = 1 если после декремента BC<>0; F3 = бит 3 операции переданный байт + A; F5 = бит 1 операции переданный байт + A
			case 0: {
				byte bt = CpuZX::memory[hl];
				write_mem8(*CpuZX::DE, bt); (*CpuZX::DE) += d; (*CpuZX::HL) += d; (*CpuZX::BC)--;
				bool bc = *CpuZX::BC != 0;
				update_flags(F5 | FH | F3 | FPV | FN, (bc << 2) | ((bt + a) & 0b00001001));
				if(rep && bc) CpuZX::RPC -= 2;
				break;
			}
			// CPI A - [HL]; HL += d; BC--; SZ*H**1-
			// S, Z, HC из A - [HL]
			// F3 = бит 3 операции A - [HL] - HC, где HC взят из F после предыдущей операции
			// F5 = бит 1 операции A - [HL] - HC
			case 1: {
				byte bt = a - CpuZX::memory[hl];
				byte fz = GET_FZ(bt);
				(*CpuZX::HL) += d; (*CpuZX::BC)--;
				bool bc = *CpuZX::BC != 0;
				update_flags(FS | FZ | F5 | FH | F3 | FPV | FN, (bt & 128) | fz | (bc << 2) | 2);
				if(rep && bc && !fz) CpuZX::RPC -= 2;
				break;
			}
			// INI [HL] <- IN[BC]; HL += d; B--; SZ5*3***
			case 2: {
				write_mem8(hl, CpuZX::port[*CpuZX::BC]);
				(*CpuZX::HL) += ((ops & 1) ? -1 : 1); (*CpuZX::B)--;
				if(rep && *CpuZX::B) CpuZX::RPC -= 2;
				break;
			}
			// OUTI OUT[BC] <- [HL]; HL += d; B--; SZ5*3***
			case 3: {
				CpuZX::port[*CpuZX::BC] = CpuZX::memory[hl];
				(*CpuZX::HL) += ((ops & 1) ? -1 : 1); (*CpuZX::B)--;
				if(rep && *CpuZX::B) CpuZX::RPC -= 2;
				break;
			}
			default: noni();
		}
	}
}
