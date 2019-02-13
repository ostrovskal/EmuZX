
#include "stdafx.h"
#include "Z80.h"

void decodePage(ssh_b* ptr, ssh_b* dst, ssh_b* dstE, int sz, bool packed) {
	if(packed) {
		while(sz > 0 && dst <= dstE) {
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
		if(*(ssh_d*)ptr == 0x00eded00) {
			sz = sz;
		}
	} else {
		memcpy(dst, ptr, sz);
	}
}

bool loadZ80(const zxString& path) {
	ssh_b* pptr = nullptr;
	bool result = false;
	int modify = 0;
	int model = -1;
	ssh_b state1 = 0;
	ssh_b* ptr;
	long sz;

	theApp->pauseCPU(true);

	try {
		_wsopen_s(&_hf, path, _O_RDONLY | _O_BINARY, _SH_DENYRD, _S_IREAD);
		if(_hf != -1) {
			result = true;
			sz = _filelength(_hf);
			pptr = new ssh_b[sz];
			ptr = pptr;
			if(_read(_hf, ptr, sz) != sz) throw(0);
			int version = 1;
			HEAD1_Z80* z80 = (HEAD1_Z80*)ptr;
			ptr += sizeof(HEAD1_Z80);
			sz -= sizeof(HEAD1_Z80);
			HEAD2_Z80* head2 = (HEAD2_Z80*)ptr;
			state1 = z80->STATE1;
			if(state1 == 255) state1 = 1;

			if(z80->PC == 0) {
				// version 2
				ptr += sizeof(HEAD2_Z80);
				version = 2;
				int length = head2->length;
				sz -= length;
				modify = head2->emulateFlags & 128;
				if(length > 23) {
					// version 3
					version = 3;
					length += 2;
					HEAD3_Z80* head3 = (HEAD3_Z80*)ptr;
					ptr += (length - sizeof(HEAD2_Z80));

				}
			}
			if(!modify) {
				if(version == 1) model = MODEL_48K;
				else if(version == 2) {
					switch(head2->hardMode) {
						case 0: case 1: model = MODEL_48K; break;
						case 3: case 4: case 5: case 6: model = MODEL_128K; break;
						case 9: model = MODEL_PENTAGON; break;
						case 10: model = MODEL_SCORPION; break;
					}
				} else if(version == 3) {
					switch(head2->hardMode) {
						case 0: case 1: case 3: model = MODEL_48K; break;
						case 4: case 5: case 6: model = MODEL_128K; break;
						case 9: model = MODEL_PENTAGON; break;
						case 10: model = MODEL_SCORPION; break;
					}
				}
			}
			if(model == -1) throw(0);
			if(!theApp->bus.changeModel(model, *_MODEL)) throw(0);
			if(version > 1) {
				while(sz > 0) {
					ssh_w size = *(ssh_w*)ptr;
					ssh_b* pageS = nullptr;
					int szData = (size == 65535 ? 16384 : size);
					int page = *(ptr + 2);
					switch(model) {
						case MODEL_48K:
							switch(page) {
								case 4: pageS = &memZX[0x8000]; break;
								case 5: pageS = &memZX[0xc000]; break;
								case 8: pageS = &memZX[0x4000]; break;
							}
							break;
						case MODEL_128K:
						case MODEL_PENTAGON:
							if(page >= 3 && page <= 10)
								pageS = theApp->bus.getMemBank(page - 3, false);
							break;
						case MODEL_SCORPION:
							if(page >= 3 && page <= 18)
								pageS = theApp->bus.getMemBank(page - 3, false);
							break;
					}
					ptr += 3;
					if(pageS != nullptr)
						decodePage(ptr, pageS, pageS + 16384, szData, size != 65535);
					ptr += szData;
					sz -= szData;
				}
			} else {
				// RAM 48k
				decodePage(ptr, &memZX[16384], &memZX[65535], sz, (state1 & 32) == 32);
			}
			*_AF = z80->AF;
			*_IX = z80->IX; *_IY = z80->IY;
			*_BC = z80->BC; *_DE = z80->DE; *_HL = z80->HL; *_SP = z80->SP;
			*_I = z80->I; *_R = z80->R; *_IM = z80->STATE2 & 3; *_IFF1 = z80->IFF1; *_IFF2 = z80->IFF2;
			memcpy(&memZX[RC_], &z80->BC_, 8);
			(*_R) |= (state1 & 1) << 7;
			(*_PORT_FE) &= ~31;
			(*_PORT_FE) |= (state1 & 14) >> 1;
			if(version == 1) *_PC = z80->PC;
			else *_PC = head2->PC;
			if(model == MODEL_128K) {
				*_RAM = -1; *_VID = -1; *_ROM = 0;
				writePort(0xfd, 0x7f, head2->hardState);
				memcpy(&memZX[0x8000], theApp->bus.getMemBank(2, false), 16384);
			}
		}
	} catch(...) { result = false; }

	SAFE_CLOSE1(_hf);
	SAFE_DELETE(pptr);

	theApp->pauseCPU(false);

	return result;
}

bool saveZ80(const zxString& path) {
	ssh_b* pptr = nullptr;
	bool result = false;

	theApp->pauseCPU(true);

	try {
		_wsopen_s(&_hf, path, _O_CREAT | _O_TRUNC | _O_WRONLY | _O_BINARY, _SH_DENYWR, _S_IWRITE);
		if(_hf != -1) {
			result = true;
			HEAD1_Z80 z80;
			z80.AF = *_AF; z80.IX = *_IX; z80.IY = *_IY;
			z80.BC = *_BC; z80.DE = *_DE; z80.HL = *_HL; z80.SP = *_SP; z80.PC = (*_PC);
			z80.I = *_I; z80.IFF1 = *_IFF1; z80.IFF2 = *_IFF2; z80.STATE2 = *_IM; z80.R = *_R;
			memcpy(&z80.BC_, &memZX[RC_], 8);
			z80.STATE1 = ((*_R) >> 7) | (((*_PORT_FE) & 7) << 1) | 32;
			if(_write(_hf, &z80, sizeof(HEAD1_Z80)) != sizeof(HEAD1_Z80)) throw(0);

			// RAM 48k
			ssh_b* src = &memZX[16384];
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
			if(_write(_hf, pptr, sz) != sz) throw(0);
		}
	} catch(...) { result = false; }
	
	SAFE_CLOSE1(_hf);
	SAFE_DELETE(pptr);

	theApp->pauseCPU(false);

	return result;
}
