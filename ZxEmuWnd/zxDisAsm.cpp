
#include "stdafx.h"
#include "zxDisAsm.h"
#include "zxDebugger.h"

static int operands[] = {4, 2, 6, 1, 8, 1, 16, 1, 22, 2, -1, 2, -1, 2, 18, 1};

static ssh_cws namesCode[] = {	L"B", L"C", L"D", L"E", L"H", L"XH", L"YH", L"L", L"XL", L"YL", L"(HL)", L"(IX", L"(IY", L"A", L"F",
						L"BC", L"DE", L"HL", L"AF", L"SP", L"IX", L"IY", L"R", L"I", L"IM ",
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


ssh_d offsRegs[] = {RL, RXL, RYL, 0, 0, RC, RE, RL, 0, 0, RXL, RYL, 0, 0, 0, RC, RE, RC };

static ssh_b name_flags[8] = { C_NZ, C_Z, C_NC, C_C, C_PO, C_PE, C_P, C_M };
static ssh_b prefRON[] = {	C_RB, C_RC, C_RD, C_RE, C_RH, C_RL, C_PHL, C_RA,
							C_RB, C_RC, C_RD, C_RE, C_RXH, C_RXL, C_PIX, C_RA,
							C_RB, C_RC, C_RD, C_RE, C_RYH, C_RYL, C_PIY, C_RA};

static ssh_b prefAF[] = {	C_RBC, C_RBC, C_RDE, C_RDE, C_RHL, C_RHL, C_RAF, C_RAF,
							C_RBC, C_RBC, C_RDE, C_RDE, C_RIX, C_RIX, C_RAF, C_RAF,
							C_RBC, C_RBC, C_RDE, C_RDE, C_RIY, C_RIY, C_RAF, C_RAF};
static ssh_b prefSP[] = {	C_RBC, C_RBC, C_RDE, C_RDE, C_RHL, C_RHL, C_RSP, C_RSP,
							C_RBC, C_RBC, C_RDE, C_RDE, C_RIX, C_RIX, C_RSP, C_RSP,
							C_RBC, C_RBC, C_RDE, C_RDE, C_RIY, C_RIY, C_RSP, C_RSP};

void zxDisAsm::opsCB() {
	if(groupOps == 0) {
		_DA(C_ROT_RLC + codeOps);
	} else {
		_DA(C_BIT + groupOps - 1); _DA(C_BT); _DA(codeOps); _DA(C_COMMA);
	}
	if(prefix) {
		_pc -= 2;
		fromPrefRON(6);
		_pc++;
		if(typeOps == 6) return;
		_DA(C_COMMA);
	}
	fromRON(typeOps);
}

void zxDisAsm::ops00() {
	ssh_b r;
	switch(typeOps) {
		case 0:
			_DA(codeOps > 2 ? C_JR : codeOps + C_NOP);
			if(codeOps > 1) {
				if(codeOps > 3) { _DA(C_NZ + (codeOps & 3)); _DA(C_COMMA); }
				_DA(C_NN); put16((_pc + 1) + (char)read8());
			}
			break;
		case 1:
			r = fromRP_SP(codeOps);
			if(codeOps & 1) {
				// ADD HL, RP
				_DA(C_ADD); _DA(fromRP_SP(4)); _DA(C_COMMA); _DA(r);
			} else {
				// LD RP, NN
				_DA(C_LD); _DA(r); _DA(C_COMMA); _DA(C_NN); put16(read16());
			}
			break;
		case 2: {
			_DA(C_LD);
			bool is = (codeOps & 1);
			if(codeOps < 4) {
				// LD (BC/DE), A / LD A, (BC/DE)
				r = C_PBC + (fromRP_AF(codeOps) - C_RBC);
				_DA(is ? C_RA : r); _DA(C_COMMA); _DA(is ? r : C_RA);
			} else {
				// LD A/HL/IX/IY, [nn] / LD [nn], HL/IX/IY/A
				nn = read16(); 
				r = (codeOps & 2) ? C_RA : fromRP_AF(4); 
				bool is = (codeOps & 1);
				_DA(is ? r : C_PNN); if(!is) put16(nn); _DA(C_COMMA); _DA(is ? C_PNN : r); if(is) put16(nn);
			}
			break;
		}
		// INC/DEC rp
		case 3: _DA(C_INC + (codeOps & 1)); _DA(fromRP_SP(codeOps)); break;
		// INC/DEC r
		case 4: case 5: _DA(C_INC + (typeOps & 1)); fromPrefRON(codeOps); break;
		// LD r, n
		case 6: _DA(C_LD); fromPrefRON(codeOps); _DA(C_COMMA); _DA(C_N); _DA(read8()); break;
		case 7:	_DA(C_RLCA + codeOps); break;
	}
}

void zxDisAsm::ops01() {
	if(typeOps == codeOps && codeOps == 6) {
		_DA(C_HALT);
	} else {
		_DA(C_LD);
		if(typeOps == 6) {
			// LD DDD, [HL/IX]
			fromRON(codeOps); _DA(C_COMMA); fromPrefRON(typeOps);
		} else {
			// LD [HL/IX], SSS
			fromPrefRON(codeOps); _DA(C_COMMA); if(codeOps == 6) fromRON(typeOps); else fromPrefRON(typeOps);
		}
	}
}

void zxDisAsm::ops10() {
	// ADD/ADC/SUB/SBC/AND/XOR/OP/CP r
	_DA(C_ADD + codeOps); fromPrefRON(typeOps);
}

void zxDisAsm::ops11() {
	switch(typeOps) {
		// RET CCC
		case 0: _DA(C_RETF); _DA(C_NZ + codeOps); break;
		case 1:
			switch(codeOps) {
				// RET
				case 1: _DA(C_RET); break;
				// EXX
				case 3: _DA(C_EXX); break;
				// JP HL/IX/IY
				case 5: _DA(C_JP); _DA(fromRP_AF(4)); break;
				// LD SP, HL/IX/IY
				case 7: _DA(C_LD); _DA(C_COMMA); _DA(fromRP_AF(4)); break;
				// POP RP
				default: _DA(C_POP); _DA(fromRP_AF(codeOps)); break;
			}
			break;
		// JP CCC, nn
		case 2: _DA(C_JP); _DA(C_NZ + codeOps); _DA(C_COMMA); _DA(C_NN); put16(read16()); break;
		case 3:
			switch(codeOps) {
				// JP nn
				case 0: _DA(C_JP); _DA(C_NN); put16(read16()); break;
				// prefix CB
				case 1: execute(0, PREFIX_CB); break;
				// OUT n, A
				case 2: _DA(C_OUT); _DA(C_PN); _DA(read8()); _DA(C_COMMA); _DA(C_RA); break;
				// IN A, n
				case 3: _DA(C_IN); _DA(C_RA); _DA(C_COMMA); _DA(C_PN); _DA(read8()); break;
				// EX (SP), HL/IX/IY
				case 4: _DA(C_EX_SP); _DA(fromRP_AF(4)); break;
				// EX DE, HL
				case 5: _DA(C_EX_DE); break;
				// DI/EI
				case 6: case 7: _DA(C_DI + (codeOps - 6)); break;
			}
			break;
		// CALL CCC, nn
		case 4: _DA(C_CALL); _DA(C_NZ + codeOps); _DA(C_COMMA); _DA(C_NN); put16(read16()); break;
		case 5:
			switch(codeOps) {
				// CALL nn
				case 1: _DA(C_CALL); _DA(C_NN); put16(read16()); break;
					// prefix DD/FD
				case 3: case 7:
					n = read_mem8(_pc);
					if(n == 0xDD || n == 0xFD || n == 0xED) _DA(C_IX_NONI + prefix);
					else execute(((codeOps & 4) >> 2) + 1, (n == 0xCB ? PREFIX_CB : 0));
					break;
				// prefix ED
				case 5: execute(0, PREFIX_ED); break;
				// PUSH RP
				default: _DA(C_PUSH); _DA(fromRP_AF(codeOps)); break;
			}
			break;
		// ADD/ADC/SUB/SBC/AND/XOR/OP/CP n
		case 6: _DA(C_ADD + codeOps); _DA(C_N); _DA(read8()); break;
		// RST n
		case 7: _DA(C_RST); _DA(C_N); _DA(codeOps * 8); break;
	}
}

void zxDisAsm::opsED01() {
	switch(typeOps) {
		// IN F/r, (C)
		case 0: _DA(C_IN); if(codeOps == 6) _DA(C_RF); else fromRON(codeOps); _DA(C_COMMA); _DA(C_PRC); break;
		// OUT (C), 0/r
		case 1: _DA(C_OUT); _DA(C_PRC); _DA(C_COMMA); if(codeOps == 6) _DA(C_0); else fromRON(codeOps); break;
		// ADC/SBC HL, RP
		case 2: _DA((codeOps & 1) ? C_ADC : C_SBC); _DA(C_RHL); _DA(C_COMMA); _DA(fromRP_SP(codeOps)); break;
		// LD [nn], RP / LD RP, [nn]
		case 3: {
			nn = read16();
			auto r = fromRP_SP(codeOps);
			bool is = (codeOps & 1);
			_DA(C_LD); _DA(is ? r : C_PNN); if(!is) put16(nn);
			_DA(C_COMMA); _DA(is ? C_PNN : r); if(is) put16(nn);
			break;
		}
		// NEG
		case 4: _DA(C_NEG); break;
		// RETI
		case 5: _DA(C_RETI); break;
		// IM N
		case 6: 
			_DA(C_IM); _DA(C_N); 
			switch(codeOps) {
				case 2: case 6: _DA(1); break;
				case 3: case 7: _DA(2); break;
				default: _DA(0);
			}
			break;
		case 7:
			switch(codeOps) {
				// LD R/I, A
				case 0: case 1: _DA(C_LD); _DA(codeOps ? C_RR : C_RI); _DA(C_COMMA); _DA(C_RA); break;
				// LD A, R/I
				case 2: case 3: _DA(C_LD); _DA(C_RA); _DA(C_COMMA); _DA((codeOps & 1) ? C_RR : C_RI);  break;
				// RRD/RLD
				case 4: case 5: _DA(C_RRD + (codeOps - 4)); break;
				// NOP
				default: _DA(C_NOP);
			}
			break;
	}
}

void zxDisAsm::opsED10() {
	if(codeOps < 4 || typeOps > 3) { _DA(C_ED_NONI); }
	else {
		n = ((codeOps & 1) ? C_LDD : C_LDI) + typeOps;
		_DA(n + ((codeOps & 2) ? 8 : 0));
	}
}

void zxDisAsm::opsEDXX() {
	_DA(C_ED_NONI);
}

funcDA table_ops[] = {
	&zxDisAsm::ops00, &zxDisAsm::opsCB, &zxDisAsm::opsEDXX,
	&zxDisAsm::ops01, &zxDisAsm::opsCB, &zxDisAsm::opsED01,
	&zxDisAsm::ops10, &zxDisAsm::opsCB, &zxDisAsm::opsED10,
	&zxDisAsm::ops11, &zxDisAsm::opsCB, &zxDisAsm::opsEDXX
};

void zxDisAsm::execute(int prefix1, int prefix2) {
	// ������ ��������
	readOps();
	prefix = (prefix1 << 3);
	if(prefix2 == 1 && prefix1) {
		_pc++;
		readOps();
	}
	(this->*table_ops[groupOps * 3 + prefix2])();
}

ssh_w zxDisAsm::decode(ssh_w pc, ssh_d count, ssh_d* real) {
	_pc = pc;
	if(count > cmdCount) {
		delete[] cmds; cmds = new ssh_b[count * SIZE_CMD];
		delete[] adrs; adrs = new ssh_w[count + 1];
	}
	cmdCount = count;
	memset(cmds, C_END, cmdCount * SIZE_CMD);
	if(cmdCount > 0) {
		auto old_pc = _pc;
		ssh_d c;
		for(c = 0; c < count; c++) {
			if(_pc < old_pc) break;
			pos = 0;
			adrs[c] = _pc;
			execute(0, 0);
			memcpy(cmds + c * SIZE_CMD, path, pos);
		}
		cmdCount = c;
		if(real)* real = c;
		adrs[c] = _pc;
	}
	return _pc;
}

bool zxDisAsm::save(ssh_cws path) {
	zxFile f(path, false);
	for(ssh_d i = 0; i < cmdCount; i++) {
		auto address = adrs[i];
		zxString adr(address, RFMT_OPS16, true);
		zxString code(makeCode(address, adrs[i + 1] - address));
		zxString cmd(makeCommand(i, DA_FADDR | DA_FCODE));
		ssh_cws tabs = (code.length() >= 8) ? L"\t\t" : L"\t\t\t";
		zxString str(adr + L'\t' + code + tabs + cmd + L"\r\n");
		if(!f.write(str.buffer(), (long)str.length() * 2)) return false;
	}
	return true;
}

zxString zxDisAsm::makeCommand(ssh_d num, int flags) {
	zxString command;
	if(adrs && cmds && num < cmdCount) {
		auto address = adrs[num];

		if(flags & DA_FADDR) {
			command = zxString(address, RFMT_OPS16, true);
			static ZX_BREAK_POINT bp;
			static ssh_cws type_bp[] = {L"\t", L"*\t", L"+\t", L"*+\t"};
			int isPC = theApp->debug->check(address, bp, FBP_PC);
			int isMEM = theApp->debug->check(address, bp, FBP_MEM);
			if(isPC || isMEM) command += type_bp[isPC | (isMEM << 1)]; else command += L'\t';
		}
		if(flags & DA_FCODE) {
			zxString code(makeCode(address, adrs[num + 1] - address));
			ssh_u l = code.length();
			ssh_cws tabs = (l >= 8 ? L"\t" : L"\t\t");
			command += code + tabs;
		}
		ssh_b idx;
		num *= SIZE_CMD;
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
				if(r != -1) command += zxString(nn, r, true);
			}
		}
	}
	return command;
}

zxString zxDisAsm::makeCode(ssh_w address, int length) const {
	zxString code;
	for(int i = 0; i < length; i++) code += zxString(read_mem8(address++), (i != (length - 1)) * 2, true);
	return code;
}

ssh_w zxDisAsm::cmdJump(ssh_w pc) {
	auto b = read_mem8(pc);
	if(b == 0xdd || b == 0xfd || b == 0xed) pc++;
	readOps();
	pc++; _pc--;
	if(groupOps == 3) {
		if((typeOps == 4 || (typeOps == 5 && codeOps == 1) || typeOps == 2 || typeOps == 7 || 
			(typeOps == 3 && codeOps == 0) || (typeOps == 1 && codeOps == 5))) {
			return read_mem16(pc);
		}
	} else if(groupOps == 0 && typeOps == 0 && codeOps > 1) {
		return pc + (char)read_mem8(pc++);
	}
	return pc;
}

ssh_w zxDisAsm::move(ssh_w pc, int count) {
	_pc = pc;
	if(count > 0) {
		while(count--) {
			pos = 0;
			execute(0, 0);
		}
	} else if(count < 0) {
		_pc += count;
		return _pc;
		int cc = 200;
		int mn_diff = 1000000;
		int mn_addr = 0;
		int pc_t = _pc + count;
		while(cc--) {
			int addr = cmdJump(_pc);
			if(addr < pc_t) {
				int diff = pc_t - addr;
				int mn = diff + count;
				if(mn < mn_diff) {
					mn_diff = mn;
					mn_addr = addr;
					if(mn < 30) break;
				}
			}
			pos = 0;
			execute(0, 0);
		}
		if(mn_diff > 7000) _pc += count;
		else {
			int pc;
			_pc = mn_addr;
			while(mn_diff > 0) {
				pos = 0;
				pc = _pc;
				execute(0, 0);
				mn_diff -= (_pc - pc);
				if(_pc > pc_t) { _pc = pc; break; }
			}
		}
	}
	return _pc;
}

void zxDisAsm::getCmdOperand(ssh_d num, bool isPC, bool isCall, bool isRet, int* addr1, int* addr2) {
	*addr1 = *addr2 = -1;
	if(adrs && num < cmdCount) {
		_pc = adrs[num];
		auto b = read_mem8(_pc);
		if(b == 0xdd || b == 0xfd || b == 0xed) _pc++;
		if(isPC) {
			// JP			type = 3 code = 0
			// JP XXX		type = 2
			// CALL			type = 5 code = 1
			// CALL XXX		type = 4
			// DJNZ			type = 0 code = 2
			// JR			type = 0 code = 3
			// JR XXX		type = 0 code > 3
			// RET			type = 1 code = 1
			// RET CCC		type = 0
			// RST			type = 7
			// JP HL		type = 1 code = 5
			readOps();
			if(groupOps == 3) {
				bool is = (typeOps == 2 || typeOps == 7 || (typeOps == 3 && codeOps == 0) || (typeOps == 1 && codeOps == 5));
				if(!is && isCall) is = (typeOps == 4 || (typeOps == 5 && codeOps == 1));
				if(!is && isRet) is = ((typeOps == 1 && codeOps == 1) || typeOps == 0);
				if(is) {
					if(typeOps == 7) {
						// rst
						*addr1 = codeOps << 3;
						*addr2 = _pc;
					}
					else if((typeOps == 1 && codeOps == 1) || typeOps == 0) {
						// ret
						*addr1 = read_mem16(*_SP);
						*addr2 = _pc;
					} else if(typeOps == 1 && codeOps == 5) {
						// jp hl/ix/iy
						ssh_w* reg = (b == 0xdd ? _IX : (b == 0xfd ? _IY : _HL));
						*addr1 = *addr2 = *reg;
					} else {
						*addr1 = read_mem16(_pc);
						*addr2 = (_pc + 2);
					}
				}
			} else if(groupOps == 0 && typeOps == 0 && codeOps > 1) {
				*addr1 = (_pc + (char)read_mem8(_pc++));
				*addr2 = _pc;
			}
		} else {
			// LD RP, (NN)/LD A,(NN)/LD (NN), RP/LD (NN), A
			auto b1 = read_mem8(_pc++);
			bool is = (b1 == 0x01 || b1 == 0x11 || b1 == 0x21 || b1 == 0x31 || b1 == 0x22 || b1 == 0x2a || b1 == 0x32 || b1 == 0x3a);
			if(b == 0xed) {
				b1 &= 0b01'001'011;
				is = (b1 == 0b01'000'011 || b1 == 0b01'001'011);
			}
			*addr1 = *addr2 = (is ? read_mem16(_pc) : -1);
		}
	}
}

int zxDisAsm::getCmdAddress(ssh_d num) const {
	if(adrs && num < cmdCount) {
		return adrs[num];
	}
	return -1;
}

int zxDisAsm::indexFromAddress(ssh_d pc) const {
	if(adrs) {
		for(ssh_d i = 0; i < cmdCount; i++) {
			if(adrs[i] == pc) return i;
		}
	}
	return 0;
}