
#include "stdafx.h"
#include "DecoderZX.h"

void DecoderZX::ops00_CB() {
	// 00 rot SSS. rot=110(SLL); SZ503P0C
	if(prefix) {
		write_mem8(preg, rotate(*preg, true), 6);
		if(typeOps != 6) *fromRON1(typeOps) = *preg;
	} else {
		ssh_b* reg = fromRON1(typeOps);
		write_mem8(reg, rotate(*reg, true), typeOps);
	}
}

void DecoderZX::ops01_CB() {
	// 01 bit, SSS. SSS AND 2^bit *Z513*0-
	// Z = 1 ���� ����������� ��� = 0;
	// S = 1 ���� bit = 7 � ����������� ��� = 1;
	// PV = Z
	ssh_b ret = (prefix ? *preg : *fromRON1(typeOps));
	ssh_b fz = (ret & (1 << ops)) == 0;
	ssh_b fs = ((ops == 7 && !fz) ? 128 : 0);

	update_flags(FS | FZ | F5 | FH | F3 | FPV | FN, fs | (fz << 2) | 16 | (fz << 6));
}

void DecoderZX::ops10_CB() {
	// 10 RES bit, SSS. SSS <- SSS AND NOT(2^bit)
	ssh_b val = ~(1 << ops);
	if(prefix) {
		write_mem8(preg, *preg & val, 6);
		if(typeOps != 6) *fromRON1(typeOps) = *preg;
	} else {
		ssh_b* reg = fromRON1(typeOps);
		write_mem8(reg, *reg & val, typeOps);
	}
}

void DecoderZX::ops11_CB() {
	// 11 bit SSS; SET bit, SSS; SSS <- SSS OR 2^bit
	ssh_b val = 1 << ops;
	if(prefix) {
		write_mem8(preg, *preg | val, 6);
		if(typeOps != 6) *fromRON1(typeOps) = *preg;
	} else {
		ssh_b* reg = fromRON1(typeOps);
		write_mem8(reg, *reg | val, typeOps);
	}
}

