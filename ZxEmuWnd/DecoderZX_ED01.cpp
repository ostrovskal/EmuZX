
#include "stdafx.h"
#include "DecoderZX.h"

// PV <- IFF2; SZ503*0-; ��������� ������ : ���� � ������ ���������� ������� �������� ����������, �� � PV ������ 1 �������� �������� 0. ���� ���������, ������� ������ �� ���� ������ �� ��������.

funcOps table_opsED01_XXX[] = {
	&DecoderZX::funcED01_000, &DecoderZX::funcED01_000, &DecoderZX::funcED01_000, &DecoderZX::funcED01_000, &DecoderZX::funcED01_000,
	&DecoderZX::funcED01_000, &DecoderZX::funcED01_000, &DecoderZX::funcED01_111
};

void DecoderZX::funcED01_000() {
	ssh_b val, a;
	switch(typeOps) {
		// *IN F, [C];  IN[BC]; SZ503P0-; �������� ������ �����.
		// IN DDD, [C];  DDD < -IN[BC]; SZ503P0-
		case 0:
			val = readPort(*CpuZX::BC);
			if(ops != 6) *fromRON(ops) = val;
			update_flags(FS | FZ | F5 | FH | F3 | FPV | FN, (val & 128) | GET_FZ(val) | (val & 0b00101000) | GET_FP(val));
			break;
		//	*OUT[C], 0; OUT[BC] < -0
		//	OUT[C], SSS;  OUT[BC] <- SSS
		case 1: writePort(*CpuZX::BC, (ops == 6 ? 0 : *fromRON(ops))); break;
		// ADC/SBC HL, RP; SZ***VNC
		case 2: {
			ssh_w reg = *fromRP_SP(ops);
			ssh_w nn = *CpuZX::HL;
			ssh_b fn = (!(ops & 1)) << 1;
			ssh_w d = reg + getFlag(FC);
			ssh_d val = (fn ? nn - d : nn + d);
			ssh_b fc = val > 65535;
			ssh_b fz = (val == 0) << 6;
			*CpuZX::HL = (ssh_w)val;
			val = val >> 8;
			update_flags(FS | FZ | F5 | FH | F3 | FN | FC, (val & 128) | (val & 0b00101000) | fz | calcFH(nn >> 8, d >> 8, fn) | fn | fc);
			break;
		}
		// LD [NN], RP / LD RP, [nn]; RP <- [nn]
		case 3: {
			ssh_w* reg = fromRP_SP(ops);
			ssh_w nn = read_mem16PC();
			if(ops & 1) *reg = read_mem16(nn); else write_mem16(nn, *reg);
			break;
		}
		// NEG; A <- 0 - A; SZ5H3V1C; PV=1 ���� ����� ��������� A=80; �=1 ���� ����� ��������� A <> 0
		case 4:
			a = *CpuZX::A;
			val = -a;
			*CpuZX::A = val;
			update_flags(FS | FZ | F5 | FH | F3 | FPV | FN | FC,
				(val & 128) | GET_FZ(val) | (val & 0b00101000) | calcFH(0, a, 1) | ((a >= 0x80) << 2) | 2 | (a != 0));
			break;
		// RETI; IFF1 <- IFF2; SP += 2; PC <- [SP - 2];	������� �� INT
		case 5:
			CpuZX::IFF1 = CpuZX::IFF2;
			CpuZX::PC = read_mem16(*CpuZX::SP);
			*(CpuZX::SP) += 2;
			break;
		// IM X
		case 6: CpuZX::IM = (ops & 3) - 1; break;
	}
}

void DecoderZX::funcED01_111() {
		switch(ops) {
		// LD I, A; I <- A
		case 0: flagsIR(CpuZX::RI = *CpuZX::A); break;
		// LD R, A; R <- A
		case 1: flagsIR(CpuZX::RR = *CpuZX::A); break;
		// LD A, I; A <-I
		case 2: *CpuZX::A = CpuZX::RI; flagsIR(CpuZX::RI); break;
		// LD A, R; A <- R; PV <- IFF2 SZ503*0-
		case 3: *CpuZX::A = CpuZX::RR; flagsIR(CpuZX::RR); break;
		// RRD/RLD; SZ503P0-
		case 4: case 5: {
			ssh_b* reg = fromRON1(6);
			ssh_b a = *CpuZX::A & 15;
			ssh_b h;
			if(ops == 4) {
				h = (*reg) & 15;
				*reg = ((*reg) >> 4) | (a << 4);
			} else {
				h = ((*reg) & 240) >> 4;
				*reg = ((*reg) << 4) | a;
			}
			ssh_b val = *CpuZX::A = ((*CpuZX::A & 240) | h);
			update_flags(FS | FZ | F5 | FH | F3 | FPV | FN, (val & 128) | GET_FZ(val) | (val & 0b00101000) | GET_FP(val));
			break;
		}
	}
}

