
#include "stdafx.h"
#include "zxCPU.h"

int zxCPU::opsED10() {
	if(ops < 4) return opsEDXX();
	ssh_w dir = ((ops & 1) ? -1 : 1);
	ssh_b a = *_A, bt, fz, d;
	ssh_b c = cpuZX[RC];
	ssh_b rep = (ops & 2);
	int tks = 16;
	bool is = false;
	switch(typeOps) {
		// LDI; --*0**0-
		// PV = 1 ���� ����� ���������� BC<>0; F3 = ��� 3 �������� ���������� ���� + A; F5 = ��� 1 �������� ���������� ���� + A
		case 0:
			do {
				if (is) { incrementR(); incrementR(); tks += 21; }
				write_mem8(*_DE, read_mem8(*_HL)); (*_DE) += dir; (*_HL) += dir; (*_BC)--;
				is = ((*_BC) != 0);
			} while(rep && is);
			update_flags(FH | FPV | FN, (is << 2));
			break;
			// CPI; SZ*H**1-
			// S, Z, HC �� A - [HL]
			// F3 = ��� 3 �������� A - [HL] - HC, ��� HC ���� �� F ����� ���������� ��������
			// F5 = ��� 1 �������� A - [HL] - HC
		case 1:
			do {
				if (is) { incrementR(); incrementR(); tks += 21; }
				d = read_mem8((*_HL));
				(*_HL) += dir; (*_BC)--;
				bt = a - d; fz = _FZ(bt);
				is = (*_BC) != 0;
			} while(rep && is && !fz);
			update_flags(FS | FZ | FH | FPV | FN, _FS(bt) | fz | _FH(a, d, 0, 1) | (is << 2) | 2);
			break;
			// INI [HL] <- IN[BC]; SZ5*3***
		case 2:
			do {
				if (is) { incrementR(); incrementR(); tks += 21; }
				write_mem8(*_HL, theApp->bus.readPort(c, *_B));
				(*_HL) += dir; (*_B)--;
				is = ((*_B) == 0);
			} while(rep && !is);
			update_flags(FS | FZ | FH | FPV | FN, 2 | _FZ(is));
			break;
			// OUTI OUT[BC] <- [HL]; SZ5*3***
		case 3:
			do {
				if (is) { incrementR(); incrementR(); tks += 21; }
				theApp->bus.writePort(c, *_B, read_mem8(*_HL));
				(*_HL) += dir; (*_B)--;
				is = ((*_B) == 0);
			} while(rep && !is);
			update_flags(FS | FZ | FH | FPV | FN, 2 | _FZ(is));
			break;
		default: opsEDXX(); return 8;
	}
	return tks;
}
