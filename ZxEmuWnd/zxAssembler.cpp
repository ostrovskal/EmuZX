
#include "stdafx.h"
#include "zxAssembler.h"

ssh_ws ch;

static zxAssembler::ZX_ASM_OPERAND operand_spec{0x00, FASM_NN, 0, 0, nullptr};
static zxAssembler::ZX_ASM_OPERAND* op;

zxAssembler::ZX_ASM_CMD cmds[] = {
	{0x00, 0, 3, &zxAssembler::parseLd, L"LD "},
	{0b11'000'010, 0xC3, 3, &zxAssembler::parseJump, L"JP "},
	{0b11'000'100, 0xCD, 5, &zxAssembler::parseJump, L"CALL "},
	{0b00'000'011, 0b00'000'100, 4, &zxAssembler::parseIncDec, L"INC "},
	{0b00'001'011, 0b00'000'101, 4, &zxAssembler::parseIncDec, L"DEC "},
	{0x00, 0b111, 3, &zxAssembler::parseArifth, L"CP "},
	{0x00, 0b000, 4, &zxAssembler::parseArifth, L"ADD "},
	{0x00, 0b010, 4, &zxAssembler::parseArifth, L"SUB "},
	{0x00, 0b100, 4, &zxAssembler::parseArifth, L"AND "},
	{0x00, 0b101, 4, &zxAssembler::parseArifth, L"XOR "},
	{0x00, 0b110, 3, &zxAssembler::parseArifth, L"OR "},
	{0x00, 0b001, 4, &zxAssembler::parseArifth, L"ADC "},
	{0x00, 0b011, 4, &zxAssembler::parseArifth, L"SBC "},
	{0xed, 0x4d, 4, &zxAssembler::parseSingl, L"RETI"},
	{0xed, 0b01000101, 4, &zxAssembler::parseSingl, L"RETN"},
	{0b11'000'000, 0xc9, 3, &zxAssembler::parseJump, L"RET"},
	{0b11000101, 0, 5, &zxAssembler::parsePushPop, L"PUSH "},
	{0b11000001, 0, 4, &zxAssembler::parsePushPop, L"POP "},
	{0x10, 0x10, 5, &zxAssembler::parseJump, L"DJNZ "},
	{0b00'100'000, 0x18, 3, &zxAssembler::parseJump, L"JR "},
	{0xed, 0xb0, 4, &zxAssembler::parseSingl, L"LDIR"},
	{0xed, 0xb1, 4, &zxAssembler::parseSingl, L"CPIR"},
	{0xed, 0xb2, 4, &zxAssembler::parseSingl, L"INIR"},
	{0xed, 0xb3, 5, &zxAssembler::parseSingl, L"OUTIR"},
	{0xed, 0xb8, 4, &zxAssembler::parseSingl, L"LDDR"},
	{0xed, 0xb9, 4, &zxAssembler::parseSingl, L"CPDR"},
	{0xed, 0xba, 4, &zxAssembler::parseSingl, L"INDR"},
	{0xed, 0xbb, 5, &zxAssembler::parseSingl, L"OUTDR"},
	{0xed, 0xa8, 3, &zxAssembler::parseSingl, L"LDD"},
	{0xed, 0xa9, 3, &zxAssembler::parseSingl, L"CPD"},
	{0xed, 0xaa, 3, &zxAssembler::parseSingl, L"IND"},
	{0xed, 0xab, 4, &zxAssembler::parseSingl, L"OUTD"},
	{0xed, 0xa0, 3, &zxAssembler::parseSingl, L"LDI"},
	{0xed, 0xa1, 3, &zxAssembler::parseSingl, L"CPI"},
	{0xed, 0xa2, 3, &zxAssembler::parseSingl, L"INI"},
	{0xed, 0xa3, 4, &zxAssembler::parseSingl, L"OUTI"},
	{0x00, 0xd9, 3, &zxAssembler::parseSingl, L"EXX"},
	{0x00, 0x76, 4, &zxAssembler::parseSingl, L"HALT"},
	{0xed, 0x44, 3, &zxAssembler::parseSingl, L"NEG"},
	{0x00, 0x07, 4, &zxAssembler::parseSingl, L"RLCA"},
	{0x00, 0x0f, 4, &zxAssembler::parseSingl, L"RRCA"},
	{0x00, 0x17, 3, &zxAssembler::parseSingl, L"RLA"},
	{0x00, 0x1f, 3, &zxAssembler::parseSingl, L"RRA"},
	{0x00, 0x27, 3, &zxAssembler::parseSingl, L"DAA"},
	{0x00, 0x2f, 3, &zxAssembler::parseSingl, L"CPL"},
	{0x00, 0x37, 3, &zxAssembler::parseSingl, L"SCF"},
	{0x00, 0x3f, 3, &zxAssembler::parseSingl, L"CCF"},
	{0xcb, 0b00'000'000, 4, &zxAssembler::parseCB, L"RLC "},
	{0xcb, 0b00'001'000, 4, &zxAssembler::parseCB, L"RRC "},
	{0xcb, 0b00'010'000, 3, &zxAssembler::parseCB, L"RL "},
	{0xcb, 0b00'011'000, 3, &zxAssembler::parseCB, L"RR "},
	{0xcb, 0b00'100'000, 4, &zxAssembler::parseCB, L"SLA "},
	{0xcb, 0b00'101'000, 4, &zxAssembler::parseCB, L"SRA "},
	{0xcb, 0b00'110'000, 4, &zxAssembler::parseCB, L"SLI "},
	{0xcb, 0b00'111'000, 4, &zxAssembler::parseCB, L"SRL "},
	{0x00, 0xeb, 6, &zxAssembler::parseEx, L"EX DE,"},
	{0x00, 0x08, 6, &zxAssembler::parseEx, L"EX AF,"},
	{0x00, 0xe3, 8, &zxAssembler::parseEx, L"EX (SP),"},
	{0xed, 0x67, 3, &zxAssembler::parseSingl, L"RRD"},
	{0xed, 0x6f, 3, &zxAssembler::parseSingl, L"RLD"},
	{0x00, 0x00, 3, &zxAssembler::parseSingl, L"NOP"},
	{0x00, 0xf3, 2, &zxAssembler::parseSingl, L"DI"},
	{0x00, 0xfb, 2, &zxAssembler::parseSingl, L"EI"},
	{0xcb, 0b01'000'000, 4, &zxAssembler::parseCB, L"BIT "},
	{0xcb, 0b10'000'000, 4, &zxAssembler::parseCB, L"RES "},
	{0xcb, 0b11'000'000, 4, &zxAssembler::parseCB, L"SET "},
	{0x00, 0b11'000'111, 4, &zxAssembler::parseJump, L"RST "},
	{0b01'000'000, 0xdb, 3, &zxAssembler::parsePort, L"IN "},
	{0b01'000'001, 0xd3, 4, &zxAssembler::parsePort, L"OUT "},
	{0xed, 0b01'000'110, 3, &zxAssembler::parseIm, L"IM "}
};

bool zxAssembler::skipComma() {
	bool is;
	skipSpace();
	if((is = (*text == L','))) text++;
	skipSpace();
	return is;
}

bool zxAssembler::isEOF() {
	skipSpace();
	ch = *text;
	return (ch == L'\r' || ch == L'\n' || ch == L'\0');
}

void zxAssembler::skipSpace() {
	while(true) {
		ch = *text;
		if(ch == L' ' || ch == L'\t' || ch == L'\r') {
			text++;
			continue;
		}
		break;
	}
}

zxAssembler::ZX_ASM_CMD* zxAssembler::parseLexem() {
	skipSpace();
	for(int i = 0 ; i < (sizeof(cmds) / sizeof(ZX_ASM_CMD)); i++) {
		auto cmd = &cmds[i];
		if(wcsncmp(cmd->name, text, cmd->len) == 0) {
			text += cmd->len;
			return cmd;
		}
	}
	// error - unknown command;
	return nullptr;
}

zxAssembler::ZX_ASM_OPERAND operands[] = {
	{0x00, 0b010'000, 2, FASM_FLAG, L"NC"},
	{0x00, 0b000'000, 2, FASM_FLAG, L"NZ"},
	{0x00, 0b100'000, 2, FASM_FLAG, L"PO"},
	{0x00, 0b101'000, 2, FASM_FLAG, L"PE"},
	{0x00, 0b011'000, 1, FASM_FLAG, L"C"},
	{0x00, 0b001'000, 1, FASM_FLAG, L"Z"},
	{0x00, 0b111'000, 1, FASM_FLAG, L"M"},
	{0x00, 0b110'000, 1, FASM_FLAG, L"P"},
	{0xdd, 0b100'000, 3, FASM_R, L"IXH"},
	{0xdd, 0b101'000, 3, FASM_R, L"IXL"},
	{0xfd, 0b100'000, 3, FASM_R, L"IYH"},
	{0xfd, 0b101'000, 3, FASM_R, L"IYL"},
	{0xdd, 0b110'000, 3, FASM_D | FASM_PRP1, L"(IX"},
	{0xfd, 0b110'000, 3, FASM_D | FASM_PRP1, L"(IY"},
	{0x00, 0b000'000, 4, FASM_PRP2, L"(BC)"},
	{0x00, 0b010'000, 4, FASM_PRP2, L"(DE)"},
	{0x00, 0b111'000, 4, FASM_PRP1, L"(SP)"},
	{0x00, 0b110'000, 4, FASM_PRP1, L"(HL)"},
	{0x00, 0b000'000, 2, FASM_RP, L"BC"},
	{0x00, 0b010'000, 2, FASM_RP, L"DE"},
	{0x00, 0b100'000, 2, FASM_RP, L"HL"},
	{0x00, 0b110'000, 2, FASM_RP_AF, L"AF"},
	{0xdd, 0b100'000, 2, FASM_RP, L"IX"},
	{0xfd, 0b100'000, 2, FASM_RP, L"IY"},
	{0x00, 0b110'000, 2, FASM_RP_SP, L"SP"},
	{0x00, 0b000'000, 1, FASM_R, L"B"},
	{0x00, 0b001'000, 1, FASM_R, L"C"},
	{0x00, 0b010'000, 1, FASM_R, L"D"},
	{0x00, 0b011'000, 1, FASM_R, L"E"},
	{0x00, 0b100'000, 1, FASM_R, L"H"},
	{0x00, 0b101'000, 1, FASM_R, L"L"},
	{0x00, 0b111'000, 1, FASM_R, L"A"},
	{0b01'000'111, 0b01'010'111, 1, FASM_R1, L"I"},
	{0b01'001'111, 0b01'011'111, 1, FASM_R1, L"R"},
	{0xed, 0, 3, FASM_PC, L"(C)"}
};

zxAssembler::ZX_ASM_OPERAND* zxAssembler::parseOperand(int flags) {
	ZX_ASM_OPERAND* result = nullptr;

	skipSpace();
	if(flags & (FASM_RP | FASM_R | FASM_R1 | FASM_FLAG | FASM_PRP1 | FASM_PRP2 | FASM_RP_SP | FASM_RP_AF)) {
		for(auto& o : operands) {
			if(o.flags & flags) {
				if(wcsncmp(o.name, text, o.len) == 0) {
					text += o.len;
					result = &o;
					break;
				}
			}
		}
		skipSpace();
	}
	bool is1 = (result && (result->flags & FASM_D));
	bool is2 = (!result && (flags & (FASM_NN | FASM_PNN | FASM_BIT)));
	if(is1 || is2) {
		bool bracket = is1;
		if(flags & FASM_PNN) {
			if(*text == L'(') { bracket = true; text++; skipSpace(); }
		}
		skipSpace();
		ssh_cws begin = text;
		nn = *(int*)asm_ssh_wton(text, (ssh_u)Radix::_dec, (ssh_ws*)&text);
		skipSpace();
		if(begin == text) return nullptr; // no number
		if(flags & FASM_PNN || is1) {
			if(*text != L')') {
				if(bracket) return nullptr;
			} else text++;
		}
		if(flags & FASM_NN) {
			if(nn > 65535 || nn < -128) return nullptr;
		}
		if(is1) {
			if(nn < -128 ||nn > 127) return nullptr;
		}
		if(flags & FASM_BIT) {
			if(nn > 7 || nn < 0) return nullptr;
		}
		if(!result) {
			result = &operand_spec;
			result->flags = (bracket ? FASM_PNN : FASM_NN);
		}
	}
	return result;
}

int zxAssembler::parseCommand(ssh_cws txt, int flags, int* posErr) {
	pos = 0;
	text = txt;
	auto cmd = parseLexem();
	if(cmd) {
		(this->*(cmd->pfn))(cmd);
		if(isEOF()) return pos;
	}
	// ОШИБКА - неизвестная комманда
	if(posErr) *posErr = (int)(text - txt);
	return 0;
}

void zxAssembler::parseSingl(ZX_ASM_CMD* cmd) {
	if(cmd->prefix) code[pos++] = cmd->prefix;
	code[pos++] = cmd->code;
}

void zxAssembler::parseIncDec(ZX_ASM_CMD* cmd) {
	if((op = parseOperand(FASM_RP | FASM_R | FASM_PRP1 | FASM_RP_SP | FASM_D))) {
		if(op->prefix) code[pos++] = op->prefix;
		code[pos++] = (op->flags == FASM_RP ? cmd->prefix : cmd->code) | op->type;
		if(op->prefix) code[pos++] = (ssh_b)nn;
	}
}

void zxAssembler::parseArifth(ZX_ASM_CMD* cmd) {
	if((op = parseOperand(FASM_NN | FASM_R | FASM_PRP1 | FASM_D | FASM_RP_SP | FASM_RP))) {
		if(op->flags & FASM_RP) {
			// пропустить запятую
			if(skipComma()) {
				auto op1 = parseOperand(FASM_RP | FASM_RP_SP);
				if(op1) {
					if(cmd->code == 0b000 && op->type == 0b100'000) {
						// ADD HL/IX/IY, RP
						if(op->prefix) code[pos++] = op->prefix;
						code[pos++] = (0b00'001'001 | op1->type);
					} else if((cmd->code == 0b001 || cmd->code == 0b011) && op->type == 0b100'000 && op->prefix == 0) {
						// ADC/SBC HL, RP
						code[pos++] = 0xed;
						code[pos++] = ((cmd->code == 0b001 ? 0b01'00'1010 : 0b01'00'0010) | op1->type);
					}
				}
			}
		} else if(op->flags & (FASM_R | FASM_PRP1 | FASM_NN)) {
			ssh_b cod = (op->len ? (0b10'000'000 | (op->type >> 3)) : 0b11'000'110);
			if(op->prefix) code[pos++] = op->prefix;
			code[pos++] = (cod | (cmd->code << 3));
			if(op->prefix) code[pos++] = (ssh_b)nn;
		}
	}
}

void zxAssembler::parseJump(ZX_ASM_CMD* cmd) {
	if((op = parseOperand(FASM_FLAG | FASM_NN | FASM_RP))) {
		if(op->flags == FASM_FLAG) {
			// для JR проверить флаги - только 2 бита
			if(cmd->code == 0x18 && op->type > 24) return;
			code[pos++] = (cmd->prefix | op->type);
			if(cmd->code != 0xc9) {
				// JP CCC,NN/CALL CCC,NN/JR CCC,D
				if(skipComma()) {
					if((op = parseOperand(FASM_NN))) {
						if(cmd->code == 0x18) {
							if(nn > 127) pos = 0;
							else code[pos++] = (char)nn;
						} else {
							*(ssh_w*)(code + pos) = nn;
							pos += 2;
						}
						return;
					}
				}
				pos = 0;
			}
		} else if(op->flags == FASM_RP) {
			if(op->type == 0b100'000) {
				// JP HL/IX/IY
				if(op->prefix) code[pos++] = op->prefix;
				code[pos++] = 0xe9;
			}
		} else {
			if(cmd->code != 0xc9) {
				code[pos++] = cmd->code;// C3/CD/18/10
				if(cmd->code == 0x18 || cmd->code == 0x10 || cmd->code == 0b11'000'111) {
					if(cmd->code == 0b11'000'111) {
						// RST
						if(nn & 0b11000111) pos = 0;
						else code[pos - 1] |= (ssh_b)nn;
					} else {
						// DJNZ/JR
						if(nn > 127) pos = 0;
						else code[pos++] = (char)nn;
					}
				} else {
					// JP/CALL
					*(ssh_w*)(code + pos) = nn;
					pos += 2;
				}
			}
		}
	} else if(cmd->code == 0xc9) {
		// RET
		code[pos++] = 0xc9;
	}
}

void zxAssembler::parsePushPop(ZX_ASM_CMD* cmd) {
	if((op = parseOperand(FASM_RP | FASM_RP_AF))) {
		if(op->prefix) code[pos++] = op->prefix;
		code[pos++] = (cmd->prefix | op->type);
	}
}

void zxAssembler::parseIm(ZX_ASM_CMD* cmd) {
	if((op = parseOperand(FASM_NN))) {
		if(nn >= 0 && nn < 3) code[pos++] = cmd->code | ((nn + 1) << 3);
	}
}

void zxAssembler::parsePort(ZX_ASM_CMD* cmd) {
	if((op = parseOperand(FASM_R | FASM_PC | FASM_PNN))) {
		if(skipComma()) {
			auto op1 = parseOperand(FASM_R | FASM_PC | FASM_PNN);
			if(op1) {
				int f, r;
				if(cmd->code == 0xdb) { r = op->type; f = op1->flags; } else { r = op1->type; f = op->flags; }
				// IN R, (C)/(N)
				// OUT (C)/(N), A/R
				if(f == FASM_PC) {
					code[pos++] = 0xed;
					code[pos++] = (cmd->prefix | r);
				} else {
					if(nn > 255 || nn < 0 || r != 0b111'000) return;
					code[pos++] = cmd->code;
					code[pos++] = (ssh_b)nn;
				}
			}
		}
	}
}

void zxAssembler::parseCB(ZX_ASM_CMD* cmd) {
	// ROTATE
	ssh_b ops = cmd->code;
	if(cmd->code == 0b11'000'000) {
		// BIT/SET/RES
		if((op = parseOperand(FASM_BIT))) {
			ops |= (ssh_b)(nn << 3);
			if(!skipComma()) return;
		} else return;
	}
	if((op = parseOperand(FASM_R | FASM_PRP1))) {
		ssh_b cod = (ops | (op->type >> 3));
		if(op->prefix) {
			if(skipComma()) {
				code[pos++] = op->prefix;
				code[pos++] = 0xcb;
				if(op->prefix) code[pos++] = (char)nn;
				if((op = parseOperand(FASM_R))) {
					if(op->prefix) pos = 0;
					else code[pos++] = cod;
				} else pos = 0;
			}
		} else {
			code[pos++] = 0xcb;
			code[pos++] = cod;
		}
	}
}

void zxAssembler::parseEx(ZX_ASM_CMD* cmd) {
	// EX (SP),HL/IX/IY / EX DE, HL / EX AF, AF'
	if((op = parseOperand(FASM_RP | FASM_RP_AF))) {
		if(op->type == 0b100'000) {
			if(op->prefix) {
				if(cmd->code == 0xeb) return;
				code[pos++] = op->prefix;
			}
			code[pos++] = cmd->code;
		} else if(op->type == 0b110'000 && cmd->code == 0x08) {
			if(*text != L'\'') return;
			text++;
			code[pos++] = cmd->code;
		}
	}
}

void zxAssembler::parseLd(ZX_ASM_CMD* cmd) {
	ZX_ASM_OPERAND* op1;
	if((op = parseOperand(FASM_RP | FASM_RP_SP | FASM_R | FASM_PNN | FASM_PRP1 | FASM_PRP2 | FASM_R1))) {
		if(skipComma()) {
			ssh_b cod;
			switch(op->flags) {
				case FASM_RP_SP:
				case FASM_RP:
					// LD BC/DE/HL/IX/IY/SP, (NN)/NN
					if((op1 = parseOperand(FASM_NN | FASM_PNN))) {
						cod = (op1->flags == FASM_PNN ? 0b00'101'010 : 0b00'000'001);
						if(op->prefix) { code[pos++] = op->prefix; }
						else if(op1->flags == FASM_PNN && op->type != 0b100'000) { code[pos++] = 0xed; cod = 0b01'001'011; }
						code[pos++] = (cod | op->type);
						*(ssh_w*)(code + pos) = (ssh_w)nn;
						pos += 2;
					}
					break;
				case FASM_R:
					// LD A, (NN)/(BC/DE)/RR/II
					if((op1 = parseOperand(FASM_R1 | FASM_PNN | FASM_PRP2 | FASM_PRP1 | FASM_R))) {
						if(op->type == 0b111'000) {
							if(op1->flags == FASM_R1) {
								// LD A, R/I
								code[pos++] = 0xed;
								code[pos++] = op1->type;
							} else if(op1->flags == FASM_PRP2) {
								// LD A, (BC/DE)
								code[pos++] = (0b00'001'010 | op1->type);
							} else if(op1->flags == FASM_PNN) {
								code[pos++] = 0x3a;
								*(ssh_w*)(code + pos) = (ssh_w)nn;
								pos += 2;
							}
						}
						if(op1->flags & FASM_PRP1) {
							// LD R, (HL/IX/IY)
							if(op->prefix) return;
							if(op1->prefix) code[pos++] = op1->prefix;
							code[pos++] = (0b01'000'110 | op->type);
							if(op1->prefix) code[pos++] = (char)nn;
						} else if(op1->flags == FASM_R) {
							// LD R, R
							if(op1->prefix) return;
							if(op->prefix) code[pos++] = op->prefix;
							code[pos++] = (0b01'000'000 | op->type | (op1->type >> 3));
						} else if(op1->flags == FASM_NN) {
							// LD R, NN
							if(op->prefix) code[pos++] = op->prefix;
							code[pos++] = (0b00'000'110 | op->type);
							code[pos++] = (ssh_b)nn;
						}
					}
					break;
				case FASM_PRP1 | FASM_D:
				case FASM_PRP1: {
					// LD (HL/IX/IY), NN/R
					auto d = nn;
					if((op1 = parseOperand(FASM_NN | FASM_R))) {
						if(op->prefix) code[pos++] = op->prefix;
						if(op1->flags == FASM_NN) {
							if(nn > 255 || nn < 0) pos = 0;
							else {
								// // LD (HL/IX/IY), NN
								code[pos++] = (0b00'000'110 | op->type);
								if(op->prefix) code[pos++] = (char)d;
								code[pos++] = (ssh_b)nn;
							}
						} else {
							if(op1->prefix) pos = 0;
							else {
								code[pos++] = (0b01'110'000 | (op1->type >> 3));
								if(op->prefix) code[pos++] = (char)d;
							}
						}
					}
					break;
				}
				case FASM_PRP2:
					// LD (BC/DE), A
					if((op1 = parseOperand(FASM_R))) {
						if(op1->type == 0b111'000) {
							code[pos++] = (0b00'000'010 | op->type);
						}
					}
					break;
				case FASM_PNN:
					// LD (NN), RP
					if((op1 = parseOperand(FASM_RP | FASM_RP_SP))) {
						cod = 0b00'100'010;
						if(op1->prefix) { code[pos++] = op1->prefix; }
						else { code[pos++] = 0xed; cod = 0b01'000'011; }
						code[pos++] = (cod | op1->type);
						*(ssh_w*)(code + pos) = (ssh_w)nn;
						pos += 2;
					}
					break;
				case FASM_R1:
					// LD RR/II, A
					if((op1 = parseOperand(FASM_R))) {
						if(op1->type == 0b111'000) {
							code[pos++] = 0xed;
							code[pos++] = op->prefix;
						}
					}
					break;
			}
		}
	}
}