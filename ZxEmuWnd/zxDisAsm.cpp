
#include "stdafx.h"
#include "zxDisAsm.h"

ssh_cws namesCode[] = {	L"B", L"C", L"D", L"E", L"H", L"IXH", L"IYH", L"L", L"IXL", L"IYL", L"(HL)", L"(IX", L"(IY", L"A", L"F",
						L"BC", L"DE", L"HL", L"AF", L"SP", L"IX", L"IY", L"R", L"I", L"IM",
						L"(BC)", L"(DE)", L"d", L"n", L"nn", L", ",
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
						L"0", L"(C)", L"(nn)", L"(n)", L"addr", L"paddr" };

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
		n = codeOps; DA_PUT(C_BIT + groupOps - 1); DA_PUT(C_N); DA_PUT(C_COMMA);
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
				n = read8(); DA_PUT(C_N); adr = (_pc + (char)n); DA_PUT(C_ADDR); 
			}
			break;
		case 1:
			r = fromRP_SP(codeOps);
			if(codeOps & 1) {
				// ADD HL, RP
				DA_PUT(C_ADD); DA_PUT(fromRP_SP(4)); DA_PUT(C_COMMA); DA_PUT(r);
			} else {
				// LD RP, NN
				nn = read16(); DA_PUT(C_LD); DA_PUT(r); DA_PUT(C_COMMA); DA_PUT(C_NN);
			}
			break;
		case 2:
			DA_PUT(C_LD);
			if(codeOps < 4) {
				// LD (BC/DE), A / LD A, (BC/DE)
				r = C_PBC + (fromRP_AF(codeOps) - C_RBC);
				DA_PUT((codeOps & 1) ? C_RA : r); DA_PUT(C_COMMA); DA_PUT((codeOps & 1) ? r : C_RA);
			} else {
				// LD A/HL/IX/IY, [nn] / LD [nn], HL/IX/IY/A
				nn = read16(); r = (codeOps & 2) ? C_RA : fromRP_AF(4);
				DA_PUT((codeOps & 1) ? r : C_PNN); DA_PUT(C_COMMA); DA_PUT((codeOps & 1) ? C_PNN : r);
			}
			break;
		// INC/DEC rp
		case 3: DA_PUT(C_INC + (codeOps & 1)); DA_PUT(fromRP_SP(codeOps)); break;
		// INC/DEC r
		case 4: case 5: DA_PUT(C_INC + (typeOps & 1)); fromRON(codeOps); break;
		// LD r, n
		case 6: DA_PUT(C_LD); fromRON(codeOps); DA_PUT(C_COMMA); DA_PUT(C_N); n = read8(); break;
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
		case 3:
			switch(codeOps) {
				// JP nn
				case 0: nn = read16(); DA_PUT(C_JP); DA_PUT(C_NN); break;
				// prefix CB
				case 1: execute(0, PREFIX_CB); break;
				// OUT n, A
				case 2: n = read8(); DA_PUT(C_OUT); DA_PUT(C_PN); DA_PUT(C_COMMA); DA_PUT(C_RA); break;
				// IN A, n
				case 3: n = read8(); DA_PUT(C_IN); DA_PUT(C_RA); DA_PUT(C_COMMA); DA_PUT(C_PN); break;
				// EX (SP), HL/IX/IY
				case 4: DA_PUT(C_EX_SP); DA_PUT(fromRP_AF(4)); break;
				// EX DE, HL
				case 5: DA_PUT(C_EX_DE); break;
				// DI/EI
				case 6: case 7: DA_PUT(C_DI + (codeOps - 6)); break;
			}
			break;
		case 5:
			switch(codeOps) {
				// CALL nn
				case 1: nn = read16(); DA_PUT(C_CALL); DA_PUT(C_NN); break;
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
		// RET CCC
		case 0: DA_PUT(C_RETF); DA_PUT(C_NZ + codeOps); break;
		// JP CCC, nn
		case 2: DA_PUT(C_JP); DA_PUT(C_NZ + codeOps); DA_PUT(C_COMMA); DA_PUT(C_NN); break;
		// CALL CCC, nn
		case 4: DA_PUT(C_CALL); DA_PUT(C_NZ + codeOps); DA_PUT(C_COMMA); DA_PUT(C_NN); break;
		// ADD/ADC/SUB/SBC/AND/XOR/OP/CP n
		case 6: n = read8(); DA_PUT(C_ADD + codeOps); DA_PUT(C_N); break;
		// RST n
		case 7: n = codeOps * 8; DA_PUT(C_RST); DA_PUT(C_N); break;
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
			DA_PUT(C_LD);
			DA_PUT((codeOps & 1) ? C_PNN : r); DA_PUT(C_COMMA); DA_PUT((codeOps & 1) ? r : C_PNN);
			break;
		}
		// NEG
		case 4: DA_PUT(C_NEG); break;
		// RETI
		case 5: DA_PUT(C_RETI); break;
		// IM N
		case 6: n = codeOps & 3; DA_PUT(C_IM); DA_PUT(C_N); break;
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
	if(codeOps < 4) DA_PUT(C_ED_NONI);
	else {
		n = (codeOps & 1) ? C_LDD : C_LDI;
		DA_PUT(n + ((codeOps & 2) ? 8 : 0));
	}
}

void zxDisAsm::opsEDXX() {
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

ssh_w zxDisAsm::decode(ssh_w pc, int count, bool isDecimal) {
	static ssh_cws radix[] = {	L"%02X", L"%03d", L"%02X ", L"%03d ", 
								L"(#%04X)", L"(%05d)", L"(#%02X)", L"(%03d)", 
								L" + #%02X)", L"+%03d)", L"%04X", L"%05d",
								L"[#%04X]", L"[%05d]", L"{#%02X}", L"{%03d}"};
	static ssh_ws strNum[32];

	_pc = pc;
	if(count > cmdCount) {
		delete cmds; cmds = new StringZX[count];
		delete adrs; adrs = new StringZX[count];
		delete codes; codes = new StringZX[count];
	}
	cmdCount = count;
	ssh_w dec = isDecimal;

	for(int c = 0 ; c < count ; c++) {
		pos = 0;
		posOps = 0;
		swprintf_s(strNum, 32, radix[dec + 10], _pc);
		adrs[c] = strNum;
		execute(0, 0);
		// сформировать
		StringZX code;
		for(int i = 0; i < posOps; i++) {
			ssh_cws fmt = radix[dec + ((i != (posOps - 1)) * 2)];
			swprintf_s(strNum, 32, fmt, path_ops[i]);
			code += strNum;
		}
		StringZX command;
		for(int i = 0; i < pos; i++) {
			auto idx = path[i];
			int ret = -1;
			switch(idx) {
				case C_ADDR:
					ret = swprintf_s(strNum, 32, radix[dec + 12], adr);
					break;
				case C_PADDR:
					ret = swprintf_s(strNum, 32, radix[dec + 14], memZX[adr]);
					break;
				case C_N:
					ret = swprintf_s(strNum, 32, radix[dec], n);
					break;
				case C_NN:
					ret = swprintf_s(strNum, 32, radix[dec + 10], nn);
					break;
				case C_PNN:
					ret = swprintf_s(strNum, 32, radix[dec + 4], nn);
					break;
				case C_PN:
					ret = swprintf_s(strNum, 32, radix[dec + 6], n);
					break;
				case C_D:
					ret = swprintf_s(strNum, 32, radix[dec + 8], (char)d);
					break;
				default:
					command += namesCode[idx];
			}
			if(ret != -1) command += strNum;
		}
		cmds[c] = command;
		codes[c] = code;
	}
	return _pc;
}

bool zxDisAsm::save(ssh_cws path) {
	int h = 0;

	bool result = true;

	try {
		_wsopen_s(&h, path, _O_CREAT | _O_TRUNC | _O_WRONLY | _O_BINARY, _SH_DENYWR, _S_IWRITE);
		if(h) {
			for(int i = 0; i < cmdCount; i++) {
				ssh_cws tabs = (codes[i].length() >= 8) ? L"\t\t" : L"\t\t\t";
				StringZX str(adrs[i] + L'\t' + codes[i] + tabs + cmds[i] + L"\r\n");
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