
#include "stdafx.h"
#include "zxCPU.h"

void zxCPU::opsCB00() {
	// 00 rot SSS. rot=110(SLL); SZ503P0C
	if(prefix) {
		write_mem8(preg, rotate(*preg, 255));
		if(typeOps != 6) *fromRON(typeOps) = *preg;
	} else {
		ssh_b* reg = fromRON(typeOps);
		write_mem8(reg, rotate(*reg, 255));
	}
}

void zxCPU::opsCB01() {
	// 01 bit, SSS. SSS AND 2^bit *Z513*0-
	// Z = 1 если проверяемый бит = 0;
	// S = 1 если bit = 7 и проверяемый бит = 1;
	// PV = Z
	ssh_b ret = (prefix ? *preg : *fromRON(typeOps));
	ssh_b fz = (ret & (1 << ops)) == 0;
	ssh_b fs = ((ops == 7 && !fz) ? 128 : 0);

	update_flags(FS | FZ | FH | FN, fs | (fz << 2) | 16 | (fz << 6));
}

void zxCPU::opsCB10() {
	// 10 RES bit, SSS. SSS <- SSS AND NOT(2^bit)
	ssh_b val = ~(1 << ops);
	if(prefix) {
		write_mem8(preg, *preg & val);
		if(typeOps != 6) *fromRON(typeOps) = *preg;
	} else {
		ssh_b* reg = fromRON(typeOps);
		write_mem8(reg, *reg & val);
	}
}

void zxCPU::opsCB11() {
	// 11 bit SSS; SET bit, SSS; SSS <- SSS OR 2^bit
	ssh_b val = 1 << ops;
	if(prefix) {
		write_mem8(preg, *preg | val);
		if(typeOps != 6) *fromRON(typeOps) = *preg;
	} else {
		ssh_b* reg = fromRON(typeOps);
		write_mem8(reg, *reg | val);
	}
}

