
#include "stdafx.h"
#include "zxDisAsm.h"
#include "zxDebugger.h"

static int operands[] = {4, 2, 6, 1, 8, 1, 16, 1, 10, 2, -1, 2, -1, 2, 18, 1};

ssh_cws radix[] = {	L"%02X", L"%03d", L"%02X ", L"%03d ",
					L"(#%04X)", L"(%05d)", L"(#%02X)", L"(%03d)",
					L"+#%02X)", L"%+03d)", L"%04X", L"%05d",
					L"[#%04X]", L"[%05d]", L"{#%02X}", L"{%03d}",
					L"#%02X", L"%d", L"%X", L"%d",
					L"-#%02X)", L"-%02d", L"#%04X", L"%05d"};

ssh_cws namesCode[] = {	L"B", L"C", L"D", L"E", L"H", L"IXH", L"IYH", L"L", L"IXL", L"IYL", L"(HL)", L"(IX", L"(IY", L"A", L"F",
						L"BC", L"DE", L"HL", L"AF", L"SP", L"IX", L"IY", L"R", L"I", L"IM",
						L"(BC)", L"(DE)", 
						L"NZ", L"Z", L"NC", L"C", L"PO", L"PE", L"P", L"M",
						L"NOP", L"EX AF, AF'", L"DJNZ ", L"JR ",
						L"RLCA", L"RRCA", L"RLA", L"RRA", L"DAA", L"CPL", L"SCF", L"CCF",
						L"DI", L"EI",
						L"ADD ", L"ADC ", L"SUB ", L"SBC ", L"AND ", L"XOR ", L"OR ", L"CP ",
						L"RLC ", L"RRC ", L"RL ", L"RR ", L"SLA ", L"SRA ", L"SLI ", L"SRL ",
						L"BIT ", L"RES ", L"SET ",
						L"INC ", L"DEC ",
						L"RRD", L"RLD",
						L"LDI", L"CPI", L"INI", L"OUTI",
						L"LDD", L"CPD", L"IND",  L"OUTD",
						L"LDIR", L"CPIR", L"INIR", L"OUTIR",
						L"LDDR", L"CPDR", L"INDR",  L"OUTDR",

						L"EXX", L"EX DE, HL", L"EX (SP), ", L"LD ", L"JP ", L"CALL ",
						L"RET", L"RET ", L"RETI", L"RETN", L"RST ", L"PUSH ", L"POP ",
						L"HALT", L"NEG", L"IN ", L"OUT ", L"*IX*", L"*IY*", L"*ED*",
						L", ", L"", L"0", L"(C)", L"(nn)", L"(n)", L"d", L"n", L"nn", L"addr", L"paddr", L"bit" };


ssh_b offsRegs[] = {RL, RIXL, RIYL, 0, 0, RC, RE, RL, 0, 0, RIXL, RIYL, 0, 0, 0, RC, RE, RC };
ssh_b name_flags[8] = { C_NZ, C_Z, C_NC, C_C, C_PO, C_PE, C_P, C_M };
ssh_b prefRON[] = {	C_RB, C_RC, C_RD, C_RE, C_RH, C_RL, C_PHL, C_RA,
					C_RB, C_RC, C_RD, C_RE, C_RIXH, C_RIXL, C_PIX, C_RA,
					C_RB, C_RC, C_RD, C_RE, C_RIYH, C_RIYL, C_PIY, C_RA};

ssh_b prefAF[] = {	C_RBC, C_RBC, C_RDE, C_RDE, C_RHL, C_RHL, C_RAF, C_RAF,
					C_RBC, C_RBC, C_RDE, C_RDE, C_RIX, C_RIX, C_RAF, C_RAF,
					C_RBC, C_RBC, C_RDE, C_RDE, C_RIY, C_RIY, C_RAF, C_RAF};
ssh_b prefSP[] = {	C_RBC, C_RBC, C_RDE, C_RDE, C_RHL, C_RHL, C_RSP, C_RSP,
					C_RBC, C_RBC, C_RDE, C_RDE, C_RIX, C_RIX, C_RSP, C_RSP,
					C_RBC, C_RBC, C_RDE, C_RDE, C_RIY, C_RIY, C_RSP, C_RSP};

void zxDisAsm::opsCB() {
	if(groupOps == 0) {
		DA_PUT(C_ROT_RLC + codeOps);
	} else {
		DA_PUT(C_BIT + groupOps - 1); DA_PUT(C_BT); DA_PUT(codeOps); DA_PUT(C_COMMA);
	}
	if(prefix) {
		_pc -= 2;
		fromRON(6);
		_pc++;
		if(typeOps == 6) return;
	}
	fromRON1(typeOps);
}

void zxDisAsm::ops00() {
	ssh_b r;
	switch(typeOps) {
		case 0:
			DA_PUT(codeOps > 2 ? C_JR : codeOps + C_NOP);
			if(codeOps > 1) {
				if(codeOps > 3) { DA_PUT(C_NZ + (codeOps & 3)); DA_PUT(C_COMMA); }
				DA_PUT(C_NN); put16((_pc + 1) + (char)read8());
			}
			break;
		case 1:
			r = fromRP_SP(codeOps);
			if(codeOps & 1) {
				// ADD HL, RP
				DA_PUT(C_ADD); DA_PUT(fromRP_SP(4)); DA_PUT(C_COMMA); DA_PUT(r);
			} else {
				// LD RP, NN
				DA_PUT(C_LD); DA_PUT(r); DA_PUT(C_COMMA); DA_PUT(C_NN); put16(read16());
			}
			break;
		case 2: {
			DA_PUT(C_LD);
			bool is = (codeOps & 1);
			if(codeOps < 4) {
				// LD (BC/DE), A / LD A, (BC/DE)
				r = C_PBC + (fromRP_AF(codeOps) - C_RBC);
				DA_PUT(is ? C_RA : r); DA_PUT(C_COMMA); DA_PUT(is ? r : C_RA);
			} else {
				// LD A/HL/IX/IY, [nn] / LD [nn], HL/IX/IY/A
				nn = read16(); 
				r = (codeOps & 2) ? C_RA : fromRP_AF(4); 
				bool is = (codeOps & 1);
				DA_PUT(is ? r : C_PNN); if(!is) put16(nn); DA_PUT(C_COMMA); DA_PUT(is ? C_PNN : r); if(is) put16(nn);
			}
			break;
		}
		// INC/DEC rp
		case 3: DA_PUT(C_INC + (codeOps & 1)); DA_PUT(fromRP_SP(codeOps)); break;
		// INC/DEC r
		case 4: case 5: DA_PUT(C_INC + (typeOps & 1)); fromRON(codeOps); break;
		// LD r, n
		case 6: DA_PUT(C_LD); fromRON(codeOps); DA_PUT(C_COMMA); DA_PUT(C_N); DA_PUT(read8()); break;
		case 7:	DA_PUT(C_RLCA + codeOps); break;
	}
}

void zxDisAsm::ops01() {
	if(typeOps == codeOps && codeOps == 6) {
		DA_PUT(C_HALT);
	} else {
		DA_PUT(C_LD);
		if(typeOps == 6) {
			// LD DDD, [HL/IX]
			fromRON1(codeOps); DA_PUT(C_COMMA); fromRON(typeOps);
		} else {
			// LD [HL/IX], SSS
			fromRON(codeOps); DA_PUT(C_COMMA); if(codeOps == 6) fromRON1(typeOps); else fromRON(typeOps);
		}
	}
}

void zxDisAsm::ops10() {
	// ADD/ADC/SUB/SBC/AND/XOR/OP/CP r
	DA_PUT(C_ADD + codeOps); fromRON(typeOps);
}

void zxDisAsm::ops11() {
	switch(typeOps) {
		// RET CCC
		case 0: DA_PUT(C_RETF); DA_PUT(C_NZ + codeOps); break;
		case 1:
			switch(codeOps) {
				// RET
				case 1: DA_PUT(C_RET); break;
				// EXX
				case 3: DA_PUT(C_EXX); break;
				// JP HL/IX/IY
				case 5: DA_PUT(C_JP); DA_PUT(fromRP_AF(4)); break;
				// LD SP, HL/IX/IY
				case 7: DA_PUT(C_LD); DA_PUT(C_COMMA); DA_PUT(fromRP_AF(4)); break;
				// POP RP
				default: DA_PUT(C_POP); DA_PUT(fromRP_AF(codeOps)); break;
			}
			break;
		// JP CCC, nn
		case 2: DA_PUT(C_JP); DA_PUT(C_NZ + codeOps); DA_PUT(C_COMMA); DA_PUT(C_NN); put16(read16()); break;
		case 3:
			switch(codeOps) {
				// JP nn
				case 0: DA_PUT(C_JP); DA_PUT(C_NN); put16(read16()); break;
				// prefix CB
				case 1: execute(0, PREFIX_CB); break;
				// OUT n, A
				case 2: DA_PUT(C_OUT); DA_PUT(C_PN); DA_PUT(read8()); DA_PUT(C_COMMA); DA_PUT(C_RA); break;
				// IN A, n
				case 3: DA_PUT(C_IN); DA_PUT(C_RA); DA_PUT(C_COMMA); DA_PUT(C_PN); DA_PUT(read8()); break;
				// EX (SP), HL/IX/IY
				case 4: DA_PUT(C_EX_SP); DA_PUT(fromRP_AF(4)); break;
				// EX DE, HL
				case 5: DA_PUT(C_EX_DE); break;
				// DI/EI
				case 6: case 7: DA_PUT(C_DI + (codeOps - 6)); break;
			}
			break;
		// CALL CCC, nn
		case 4: DA_PUT(C_CALL); DA_PUT(C_NZ + codeOps); DA_PUT(C_COMMA); DA_PUT(C_NN); put16(read16()); break;
		case 5:
			switch(codeOps) {
				// CALL nn
				case 1: DA_PUT(C_CALL); DA_PUT(C_NN); put16(read16()); break;
					// prefix DD/FD
				case 3: case 7:
					n = memZX[_pc];
					if(n == 0xDD || n == 0xFD || n == 0xED) DA_PUT(C_IX_NONI + prefix);
					else execute(((codeOps & 4) >> 2) + 1, (n == 0xCB ? PREFIX_CB : 0));
					break;
				// prefix ED
				case 5: execute(0, PREFIX_ED); break;
				// PUSH RP
				default: DA_PUT(C_PUSH); DA_PUT(fromRP_AF(codeOps)); break;
			}
			break;
		// ADD/ADC/SUB/SBC/AND/XOR/OP/CP n
		case 6: DA_PUT(C_ADD + codeOps); DA_PUT(C_N); DA_PUT(read8()); break;
		// RST n
		case 7: DA_PUT(C_RST); DA_PUT(C_N); DA_PUT(codeOps * 8); break;
	}
}

void zxDisAsm::opsED01() {
	switch(typeOps) {
		// IN F/r, (C)
		case 0: DA_PUT(C_IN); if(codeOps == 6) DA_PUT(C_RF); else fromRON1(codeOps); DA_PUT(C_COMMA); DA_PUT(C_PRC); break;
		// OUT (C), 0/r
		case 1: DA_PUT(C_OUT); DA_PUT(C_PRC); DA_PUT(C_COMMA); if(codeOps == 6) DA_PUT(C_0); else fromRON1(codeOps); break;
		// ADC/SBC HL, RP
		case 2: DA_PUT((codeOps & 1) ? C_ADC : C_SBC); DA_PUT(C_RHL); DA_PUT(C_COMMA); DA_PUT(fromRP_SP(codeOps)); break;
		// LD [nn], RP / LD RP, [nn]
		case 3: {
			nn = read16();
			auto r = fromRP_SP(codeOps);
			bool is = (codeOps & 1);
			DA_PUT(C_LD); DA_PUT(is ? C_PNN : r); if(is) put16(nn);
			DA_PUT(C_COMMA); DA_PUT(is ? r : C_PNN); if(!is) put16(nn);
			break;
		}
		// NEG
		case 4: DA_PUT(C_NEG); break;
		// RETI
		case 5: DA_PUT(C_RETI); break;
		// IM N
		case 6: DA_PUT(C_IM); DA_PUT(C_N); DA_PUT(codeOps & 3); break;
		case 7:
			switch(codeOps) {
				// LD R/I, A
				case 0: case 1: DA_PUT(C_LD); DA_PUT(codeOps ? C_RR : C_RI); DA_PUT(C_COMMA); DA_PUT(C_RA); break;
				// LD A, R/I
				case 2: case 3: DA_PUT(C_LD); DA_PUT(C_RA); DA_PUT(C_COMMA); DA_PUT((codeOps & 1) ? C_RR : C_RI);  break;
				// RRD/RLD
				case 4: case 5: DA_PUT(C_RRD + (codeOps - 4)); break;
				// NOP
				default: DA_PUT(C_NOP);
			}
			break;
	}
}

void zxDisAsm::opsED10() {
	if(codeOps < 4) { DA_PUT(C_ED_NONI); _pc--; }
	else {
		n = (codeOps & 1) ? C_LDD : C_LDI;
		DA_PUT(n + ((codeOps & 2) ? 8 : 0));
	}
}

void zxDisAsm::opsEDXX() {
	_pc--;
	DA_PUT(C_ED_NONI);
}

funcDA table_ops[] = {
	&zxDisAsm::ops00, &zxDisAsm::opsCB, &zxDisAsm::opsEDXX,
	&zxDisAsm::ops01, &zxDisAsm::opsCB, &zxDisAsm::opsED01,
	&zxDisAsm::ops10, &zxDisAsm::opsCB, &zxDisAsm::opsED10,
	&zxDisAsm::ops11, &zxDisAsm::opsCB, &zxDisAsm::opsEDXX
};

void zxDisAsm::execute(int prefix1, int prefix2) {
	// читаем операцию
	readOps();
	prefix = prefix1;
	if(prefix2 == 1 && prefix1) {
		_pc++;
		readOps();
	}
	(this->*table_ops[groupOps * 3 + prefix2])();

}

ssh_w zxDisAsm::decode(ssh_w pc, ssh_d count) {
	_pc = pc;
	if(count > cmdCount) {
		delete cmds; cmds = new ssh_b[count * 12];
		delete adrs; adrs = new ssh_w[count + 1];
	}
	cmdCount = count;
	memset(cmds, C_END, cmdCount * 12);

	for(ssh_d c = 0; c < count; c++) {
		pos = 0;
		adrs[c] = _pc;
		execute(0, 0);
		memcpy(cmds + c * 12, path, pos);
	}
	adrs[count] = _pc;
	return _pc;
}

bool zxDisAsm::save(ssh_cws path, ssh_b dec) {
	int h = 0;

	bool result = true;

	try {
		_wsopen_s(&h, path, _O_CREAT | _O_TRUNC | _O_WRONLY | _O_BINARY, _SH_DENYWR, _S_IWRITE);
		if(h) {
			for(ssh_d i = 0; i < cmdCount; i++) {
				auto address = adrs[i];
				StringZX adr(fromNum(address, radix[dec + 10]));
				StringZX code(makeCode(address, adrs[i + 1] - address, dec));
				StringZX cmd(makeCommand(i, (dec & 1) | DA_FADDR));
				ssh_cws tabs = (code.length() >= 8) ? L"\t\t" : L"\t\t\t";
				StringZX str(adr + L'\t' + code + tabs + cmd + L"\r\n");
				int sz = (int)str.length() * 2;
				if(_write(h, str.buffer(), sz) != sz) throw(0);
			}
		}
	} catch(...) {
		result = false;
	}
	if(h) _close(h);
	return result;
}

StringZX zxDisAsm::makeCommand(ssh_d num, int flags) {
	StringZX command;
	if(adrs && cmds && num < cmdCount) {
		auto address = adrs[num];
		ssh_b dec = (flags & DA_FDEC);

		if((flags & DA_FADDR)) {
			command = StringZX::fmt(radix[dec + 10], address);
			static ZX_BREAK_POINT bp;
			static ssh_cws type_bp[] = {L" ", L"*", L"+", L"*+"};
			int isPC = theApp.debug->check(address, bp, FBP_PC);
			int isMEM = theApp.debug->check(address, bp, FBP_MEM);
			if(isPC || isMEM) {
				command += type_bp[isPC | (isMEM << 1)];
			} else command += L' ';
		}
		if((flags & DA_FCODE) == DA_FCODE) {
			StringZX code(makeCode(address, adrs[num + 1] - address, dec));
			ssh_cws tabs = (code.length() >= 8) ? L"\t" : L"\t\t";
			command += L'\t' + code + tabs;
		}
		ssh_b idx;
		num *= 12;
		while((idx = cmds[num++]) != C_END) {
			if(idx < C_PNN) {
				command += namesCode[idx];
			} else {
				int offs = (idx - C_PNN) * 2;
				int r = operands[offs];
				int o = operands[offs + 1];;
				nn = (o == 1 ? cmds[num] : *(ssh_w*)(cmds + num));
				num += o;
				switch(idx) {
					case C_ADDR:if((flags & DA_FPADDR)) r = 12; break;
					case C_D: if(nn > 0x7f) { nn = 256 - nn; r = 20; } break;
				}
				if(r != -1) {
					swprintf_s(tmpBuf, 32, radix[dec + r], nn);
					command += tmpBuf;
				}
			}
		}
	}
	return command;
}

StringZX zxDisAsm::makeCode(ssh_w address, int length, ssh_b dec) const {
	StringZX code;
	for(int i = 0; i < length; i++) {
		code += fromNum(memZX[address++], radix[dec + ((i != (length - 1)) * 2)]);
	}
	return code;
}
ssh_w zxDisAsm::move(ssh_w pc, int count) {
	_pc = pc;
	if(count > 0) {
		while(count--) {
			pos = 0;
			execute(0, 0);
		}
	} else if(count < 0){
		_pc += count;
		// TODO: сдвиг дизасма назад
	}
	return _pc;
}

int zxDisAsm::getCmdOperand(ssh_d num, bool isPC) {
	if(adrs && num < cmdCount) {
		_pc = adrs[num];
		readOps();
		if(isPC) {
			// JP XXX, JP, CALL, CALL XXX, JR CC, JR. DJNZ
			if(groupOps == 3) {
				bool is = (typeOps == 2 || typeOps == 4) || (typeOps == 3 && codeOps == 0) || (typeOps == 5 && codeOps == 1);
				if(is) return *(ssh_w*)(memZX + _pc);
			} else if(groupOps == 0 && typeOps == 0 && codeOps > 1) {
				return ((_pc + 1) + (char)memZX[_pc]);
			}
		} else {
			// LD RP, NN/LD NN, RP/LD A/RP,[NN]/LD [NN], A/RP
		}
	}
	return -1;
}

int zxDisAsm::getCmdAddress(ssh_d num) const {
	if(adrs && num < cmdCount) {
		return adrs[num];
	}
	return -1;
}

bool zxDisAsm::isCALL(ssh_d num) {
	if(adrs && num < cmdCount) {
		_pc = adrs[num];
		if(readOps() == 3) return (typeOps == 4 || (typeOps == 5 && codeOps == 1));
	}
	return false;

}

int zxDisAsm::indexFromAddress(ssh_d pc) const {
	if(adrs) {
		for(ssh_d i = 0; i < cmdCount; i++) {
			if(adrs[i] == pc) return i;
		}
	}
	return 0;
}