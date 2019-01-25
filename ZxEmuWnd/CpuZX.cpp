
#include "stdafx.h"
#include "CpuZX.h"
#include "DecoderZX.h"
#include "SettingsZX.h"

extern SettingsZX settings;
extern ssh_cws nameROMs[];

ssh_b CpuZX::IFF1 = 0;
ssh_b CpuZX::IFF2 = 0;
ssh_b CpuZX::trap = 0;

bool CpuZX::isExec = false;

ssh_b CpuZX::STATE = 0;

ssh_b CpuZX::portFE = 224;

ssh_b CpuZX::RI;

ssh_b CpuZX::RR;

ssh_b CpuZX::IM;

ssh_w CpuZX::PC;

ssh_b* CpuZX::A;
ssh_b* CpuZX::B;

ssh_w* CpuZX::BC;
ssh_w* CpuZX::DE;
ssh_w* CpuZX::HL;
ssh_w* CpuZX::SP;

ssh_b CpuZX::cpu[22];

ssh_b CpuZX::memory[65536];

ssh_b CpuZX::port[65536];

CpuZX::CpuZX() {
	decoder = new DecoderZX();
	ROM = nullptr; szROM = 0;
	signalRESET();
}

CpuZX::~CpuZX() {
	isExec = false;
	delete decoder;
	decoder = nullptr;
}

void CpuZX::signalRESET() {
	PC = 0;
	RR = RI = IM = trap = 0;
	IFF1 = IFF2 = 0;
	STATE = 0;
	portFE = 224;

	delete ROM; ROM = nullptr;
	szROM = 0;

	memset(cpu, 0, sizeof(cpu));
	memset(memory, 0, sizeof(memory));
	memset(port, 255, sizeof(port));

	port[31] = 0;

	A = &cpu[RA];
	B = &cpu[RB];

	BC = (ssh_w*)&cpu[RC];
	DE = (ssh_w*)&cpu[RE];
	HL = (ssh_w*)&cpu[RL];
	SP = (ssh_w*)&cpu[RSPL];

	*SP = 65534;

	// грузим ПЗУ
	int hh = 0;
	try {
		StringZX pathROM(settings.mainDir + L"Roms\\" + nameROMs[settings.modelZX] + L".rom");
		_wsopen_s(&hh, pathROM, _O_RDONLY, _SH_DENYRD, _S_IREAD);
		if(hh) {
			szROM = _filelength(hh);
			ROM = new ssh_b[szROM];
			if(_read(hh, ROM, szROM) != szROM) throw(0);
		}
	} catch(...) {

	}
	if(hh) _close(hh);
	memcpy(memory, ROM, szROM >= 16384 ? 16384 : szROM);
	isExec = true;
}

void CpuZX::signalINT() {
	if(IFF1 && trap) {
		trap = IFF1 = IFF2 = 0;
		switch(IM) {
			case 0: 
				// вызываем системный монитор (debug window)
				break;
			case 1:
				decoder->execCALL(0x38);
				break;
			case 2:
				decoder->execCALL(decoder->read_mem16(RI * 256 + 254));
				break;
		}
	}
}

void CpuZX::signalNMI() {
	IFF1 = 0;
	decoder->execCALL(0x66);
}

void CpuZX::execute() {
	if(isExec) {
		STATE |= INT;
		// выполнение операции
		decoder->execOps(0, 0);
		// проверка на прерывания
		if((STATE & INT) == INT) signalINT();
	}
}

bool CpuZX::saveStateZX(const StringZX& path) {
	int h = 0;

	bool result = true;
	bool exec = isExec;
	
	isExec = false;

	try {
		_wsopen_s(&h, path, _O_CREAT | _O_TRUNC | _O_WRONLY | _O_BINARY, _SH_DENYWR, _S_IWRITE);
		if(h) {
			if(_write(h, &cpu, sizeof(cpu)) != sizeof(cpu)) throw(0);
			if(_write(h, &memory, sizeof(memory)) != sizeof(memory)) throw(0);

			ssh_b temp[32];

			temp[0] = RI; temp[1] = RR; temp[2] = IM; temp[3] = IFF1; temp[4] = IFF2;
			temp[5] = STATE; temp[6] = portFE; temp[7] = trap; temp[8] = settings.modelZX;
			*(ssh_w*)(temp + 9) = PC;

			int size =	sizeof(RI) + sizeof(RR) + sizeof(IM) + sizeof(IFF1) + sizeof(IFF2) +
						sizeof(STATE) + sizeof(portFE) + sizeof(trap) + sizeof(PC) + sizeof(settings.modelZX);
			
			if(_write(h, &temp, size) != size) throw(0);
		}
	} catch(...) {
		result = false;
	}

	if(h) _close(h);
	
	isExec = exec;

	return result;
}

bool CpuZX::loadStateZX(const StringZX& path) {
	FILE* hh = nullptr;
	bool result = true;
	bool exec = isExec;
	isExec = false;

	try {
		_wfopen_s(&hh, path, L"rb");
		if(hh) {
			if(fread(&cpu, 1, sizeof(cpu), hh) != sizeof(cpu)) throw(0);
			if(fread(&memory, 1, sizeof(memory), hh) != sizeof(memory)) throw(0);
			
			int size =	sizeof(RI) + sizeof(RR) + sizeof(IM) + sizeof(IFF1) + sizeof(IFF2) + 
						sizeof(STATE) + sizeof(portFE) + sizeof(trap) + sizeof(PC) + sizeof(settings.modelZX);
			
			ssh_b temp[32];

			if(fread(&temp, 1, size, hh) != size) throw(0);

			RI = temp[0]; RR = temp[1]; IM = temp[2]; IFF1 = temp[3]; IFF2 = temp[4];
			STATE = temp[5]; portFE = temp[6]; trap = temp[7]; settings.modelZX = temp[8];
			PC = *(ssh_w*)(temp + 9);
			
			STATE |= (BORDER | SOUND);
		}
	} catch(...) {
		result = false;
	}
	
	if(hh) fclose(hh);
	
	isExec = exec;

	return result;
}

void decodePage(ssh_b* ptr, ssh_b* dst, ssh_b* dstE, int sz, bool packed) {
	if(packed) {
		while(sz > 0) {
			ssh_b b = *ptr++;
			ssh_b c = 1;
			sz--;
			if(b == 0xed) {
				c = *ptr++;
				if(c == 0xed) {
					c = *ptr++;
					b = *ptr++;
					sz -= 3;
				} else {
					ptr--;
					c = 1;
				}
			}
			memset(dst, b, c);
			dst += c;
		}
	} else {
		memcpy(dst, ptr, sz);
	}
}

bool CpuZX::loadZ80(const StringZX& path) {
	int hh = 0;
	ssh_b* pptr = nullptr;
	bool result = true;
	bool exec = isExec;
	ssh_b* ptr;
	long sz;

	isExec = false;
	try {
		_wsopen_s(&hh, path, _O_RDONLY | _O_BINARY, _SH_DENYRD, _S_IREAD);
		if(hh) {
			sz = _filelength(hh);
			pptr = new ssh_b[sz];
			ptr = pptr;
			if(_read(hh, ptr, sz) != sz) throw(0);
			// AF, CB, LH, PC, SP
			*(ssh_w*)&cpu[RA] = *(ssh_w*)ptr;
			*BC = *(ssh_w*)(ptr + 2);
			*HL = *(ssh_w*)(ptr + 4);
			PC = *(ssh_w*)(ptr + 6);
			*SP = *(ssh_w*)(ptr + 8);
			ptr += 10;
			// I, R
			RI = *ptr++; RR = *ptr++;
			// state1
			ssh_b state1 = *ptr++;
			if(state1 == 255) state1 = 1;
			RR |= (state1 & 1) << 7;
			portFE &= ~7;
			portFE |= (state1 & 14) >> 1;
			STATE |= BORDER;
			// ED
			*DE = *(ssh_w*)ptr;
			// CB', ED', LH', AF'
			memcpy(&cpu[RC + 14], ptr + 2, 8);
			// IY, IX
			*(ssh_w*)&cpu[RIYL] = *(ssh_w*)(ptr + 10);
			*(ssh_w*)&cpu[RIXL] = *(ssh_w*)(ptr + 12);
			ptr += 14;
			// DI/EI, IFF2
			IFF1 = *ptr++; IFF2 = *ptr++;
			// state2
			ssh_b state2 = *ptr++;
			IM = state2 & 3;
			sz -= 30;
			if(PC == 0) {
				// version 2
				int len = *(ssh_w*)ptr; ptr += 2;
				PC = *(ssh_w*)ptr;
				ssh_b h_mode = *(ptr + 2);
				ssh_b h_state = *(ptr + 3);
				ssh_b i_state = *(ptr + 4);
				ssh_b emu = *(ptr + 5);
				ssh_b son = *(ptr + 6);
				byte* s_regs = ptr + 7;
				if(len > 23) {
					ptr += 23;
					// version 3
					ssh_w low_tstate = *(ssh_w*)ptr;
					ssh_b high_tstate = *(ptr + 2);
					ssh_b mgt_rom_page = *(ptr + 4);
					ssh_b multiface_rom_page = *(ptr + 5);
					ssh_b mem_0000_1fff = *(ptr + 6);
					ssh_b mem_2000_3fff = *(ptr + 7);
					ssh_b * _5x_key_maps = ptr + 8;
					ssh_b * _5x_ascii_vals = ptr + 18;
					ssh_b mgt_type = *(ptr + 28);
					ssh_b inhibitor_but = *(ptr + 29);
					ssh_b inhibitor_flg = *(ptr + 30);
					ptr += 31;
					if(len > 54) {
						ssh_b port_1ffd = *ptr++;
					}
				}
				sz -= len;
				while(sz > 0) {
					ssh_w size = *(ssh_w*)ptr;
					ssh_b* pageS = nullptr;
					int szData = (size == 65535 ? 16384 : size);
					int page = *(ptr + 2);
					switch(page) {
						case 8: pageS = &memory[16384]; break;
						case 4: pageS = &memory[0x8000]; break;
						case 5: pageS = &memory[0xc000]; break;
					}
					ptr += 3;
					if(pageS != nullptr)
						decodePage(ptr, pageS, pageS + 16384, szData, size != 65535);
					ptr += szData;
					sz -= szData;
				}
			} else {
				// RAM 48k
				decodePage(ptr, &memory[16384], &memory[65535], sz, (state1 & 32) == 32);
			}
		}
	} catch(...) {
		result = false;
	}
	if(hh) _close(hh);
	isExec = exec;
	delete pptr;
	return result;
}

bool CpuZX::saveZ80(const StringZX& path) {
	int h = 0;
	ssh_b* pptr = nullptr;
	bool result = true;
	bool exec = isExec;

	isExec = false;
	try {
		_wsopen_s(&h, path, _O_CREAT | _O_TRUNC | _O_WRONLY | _O_BINARY, _SH_DENYWR, _S_IWRITE);
		if(h) {
			ssh_b temp[30];

			*(ssh_w*)&temp[0] = *(ssh_w*)&cpu[RA];
			*(ssh_w*)&temp[2] = *BC;
			*(ssh_w*)&temp[4] = *HL;
			*(ssh_w*)&temp[6] = PC;
			*(ssh_w*)&temp[8] = *SP;
			temp[10] = RI; temp[11] = RR & 127;
			temp[12] = ((RR & 128) >> 7) | ((portFE & 7) << 1) | 32;
			*(ssh_w*)&temp[13] = *DE;
			memcpy(&temp[15], &cpu[RC + 14], 8);
			*(ssh_w*)&temp[23] = *(ssh_w*)&cpu[RIYL];
			*(ssh_w*)&temp[25] = *(ssh_w*)&cpu[RIXL];
			temp[27] = IFF1; temp[28] = IFF2;
			temp[29] = IM;
			if(_write(h, &temp, sizeof(temp)) != sizeof(temp)) throw(0);

			// RAM 48k
			ssh_b* src = &memory[16384];
			ssh_b* srcE = src + 49152;

			pptr = new ssh_b[65535];

			ssh_b* dst = pptr;
			ssh_b b1;
			int c = 1;
			while(src < srcE) {
				b1 = *src++;
				if(b1 != *src) {
					if(c > 1) {
						if(c < 5 && b1 != 0xed) {
							memset(dst, b1, c);
							dst += c;
						} else {
							*dst++ = 0xed;
							*dst++ = 0xed;
							*dst++ = c;
							*dst++ = b1;
						}
					} else {
						*dst++ = b1;
					}
				} else {
					c++;
					if(c < 256) continue;
					*dst++ = 0xed;
					*dst++ = 0xed;
					*dst++ = c - 1;
					*dst++ = b1;
				}
				c = 1;
			}
			c--;
			if(c > 1) {
				*dst++ = 0xed;
				*dst++ = 0xed;
				*dst++ = c;
				*dst++ = b1;
			} else if(c == 1) *dst++ = b1;
			ssh_d sz = (ssh_d)(dst - pptr);
			if(_write(h, pptr, sz) != sz) throw(0);
		}
	} catch(...) {
		result = false;
	}
	if(h) _close(h);
	isExec = exec;
	delete pptr;
	return result;
}
