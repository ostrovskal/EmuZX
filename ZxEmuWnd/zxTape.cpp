
#include "stdafx.h"
#include "zxTape.h"

ssh_w neproigrannaya_pausa = 0;
const short rewCounterConst = 3500000 / 44100;

int zxTape::hookPC(ssh_w pc) {
	if(pc < 16384) {
		// активность TR DOS
		if(pc == 15616 || pc == 15619) {
			modifyTSTATE((pageROM != PAGE_ROM[0] && *_MODEL > MODEL_128K) * ZX_TR_DOS, ZX_TR_DOS);
			return 4;
		}
		// перехват SAVE ""
		if(pc == 1218) {
			trapSave();
			*_PC = read_mem16(*_SP);
			(*_SP) += 2;
			return 4;
		}
		// перехват LOAD ""
		if((pc == 1366 || pc == 1378) && curr) {
			if(isTraps) {
				auto blk = curr->type;
				if(pos < len && blk->id != TBLK_DIRECT_REC && *_DE != (blk->len - 2))
					stop();
				blk = curr->type;
				if(*_DE == blk->len - 2) {
					trapLoad();
					*_PC = read_mem16(*_SP);
					(*_SP) += 2;
					return 4;
				} else {
					control(1);
				}
			} else {
				if(pos >= len) control(1);
			}
		}
	}
	return 0;
}

zxTape::zxTape() :	pos(0), len(0), isPause(true), in(0), out(0), micPos(0), micPrevPos(0), rewCounter(0), koef(0),
					recPosBuffer(DIRECT_REC_SIZE - 1), curr(nullptr),
					isTraps(true), isDirectMic(false), isReadOnly(false), isRecording(false) {
}

void zxTape::addMicBlock() {
	DIRECT_REC direct;
	ssh_i len(micPos / 8);
	direct.TS = 79;
	direct.pause = 0;
	direct.used = micPos - len * 8 + 1;
	len++;
	direct.l1 = len & 0x0000FF;
	direct.l2 = (len & 0x00FF00) >> 8;
	direct.l3 = (len & 0xFF0000) >> 16;
	addBlock(TBLK_DIRECT_REC, len, micBuf, sizeof(DIRECT_REC), &direct);
	micPos = 0;
}

void zxTape::addBlock(ssh_b id, ssh_i len, ssh_b* data, int ilen, void* info) {
	auto tmp = new BLOCK(id);
	tmp->len = len;
	tmp->ilen = ilen;
	if(len) { tmp->data = new ssh_b[len]; memcpy(tmp->data, data, len); }
	if(ilen) { tmp->info = new ssh_b[ilen]; memcpy(tmp->info, info, ilen); }
	blocks.add(tmp);
}

void zxTape::trapLoad() {
	auto blk = curr->type;
	auto len = blk->len - 2;
	auto data(blk->data + 1);
//	*_AF = 179; *_BC = 0xb001; *_HL = data[len];
	for(int i = 0; i < *_DE; i++) write_mem8(get_mem(*_IX + i), data[i]);
	*_DE -= len; *_IX += len;
	next();
	control(0);
}

void zxTape::trapSave() {
	STD_BLOCK rom;
	ssh_b a = *_A;
	// тип блока
	TMP_BUF[0] = a;
	ssh_w len = *_DE + 2;
	for(int i = 0; i < *_DE; i++) { auto b = read_mem8(*_IX + i); a ^= b; TMP_BUF[i + 1] = b; }
	TMP_BUF[*_DE + 1] = a;
	rom.pause = 1000;
	rom.len = len;
	addBlock(TBLK_STD_SPEED, len, TMP_BUF, sizeof(STD_BLOCK), &rom);
}

bool zxTape::isSkip() {
	if(curr) {
		auto id = curr->type->id;
		return (id == TBLK_TEXT_DESC || id == TBLK_ARCHIVE_INFO || id == TBLK_HARDWATE_TYPE || id == TBLK_CUSTOM_INFO || id == TBLK_VERSION);
	}
	return false;
}

bool zxTape::next() {
	bool first = false;

	if(!curr) return true;
	auto next = curr;
	curr = curr->next;
LR: if(next == curr) return true;
	if(!curr) { curr = blocks.fromPos(1); first = true; }
	if(isSkip() || curr == blocks.root()) { curr = curr->next; goto LR; }
	return first;
	if(curr) {
		curr = curr->next;
		while(isSkip()) curr = curr->next;
	}
	return curr != nullptr;
}

bool zxTape::isStd() {
	if(!isTraps || !curr) return false;
	auto blk = curr->type;
	if(blk->id != TBLK_STD_SPEED) return false;
	auto data = blk->data;
	return (data[0] == 0 || data[0] == 255);
}

ssh_b zxTape::b2bget(ssh_b* src, ssh_i pos) {
	ssh_b bn = 1 << (7 - (pos % 8));
	return ((src[pos / 8] & bn) != 0);
}

void zxTape::b2bset(ssh_b* dst, ssh_i pos, ssh_b bit) {
	ssh_i bc = pos / 8;
	ssh_b bn = (1 << (7 - (pos % 8)));
	if(!bit) dst[bc] &= ~bn; else dst[bc] |= bn;
}

bool zxTape::isExecute() {
	if((*_TSTATE & ZX_TR_DOS) || !curr) return false;
	if(isSkip()) next();
	return isStd();
}

bool zxTape::fill(ssh_b* tape, ssh_i* len, BLOCK* blk) {
	ssh_i i, j, l;
	ssh_b byte;
	ssh_w len16;
	double freq = 44100.0 / 3500000.0;

	switch(blk->id) {
		case TBLK_TURBO_SPEED: {
			auto turbo = (TURBO_BLOCK*)blk->info;
			for(i = 0; i < turbo->pilot_toneL_inPulses; i++) {
				for(j = 0; j < (freq / (double)turbo->pilotL); j++) b2bset(tape, (*len)++, koef);
				koef = 1 - koef;
				for(j = 0; j < (freq / (double)turbo->pilotL); j++) b2bset(tape, (*len)++, koef);
				koef = 1 - koef;
			}
			for(j = 0; j < (freq / (double)turbo->syncFL); j++) b2bset(tape, (*len)++, koef);
			koef = 1 - koef;
			for(j = 0; j < (freq / (double)turbo->syncSL); j++) b2bset(tape, (*len)++, koef);
			koef = 1 - koef;
			for(i = 0; i < (ssh_i)(turbo->l1 + turbo->l2 * 256 + turbo->l3 * 65536); i++) {
				byte = blk->data[i];
				for(l = 0; l < 8; l++) {
					if(b2bget(&byte, l)) {
						for(j = 0; j < (freq / (double)turbo->oneL); j++) b2bset(tape, (*len)++, koef);
						koef = 1 - koef;
						for(j = 0; j < (freq / (double)turbo->oneL); j++) b2bset(tape, (*len)++, koef);
						koef = 1 - koef;
					} else {
						for(j = 0; j < (freq / (double)turbo->zeroL); j++) b2bset(tape, (*len)++, koef);
						koef = 1 - koef;
						for(j = 0; j < (freq / (double)turbo->zeroL); j++) b2bset(tape, (*len)++, koef);
						koef = 1 - koef;
					}
				}
			}
			byte = blk->data[i];
			for(l = 0; l < turbo->used_bits; l++) {
				if(b2bget(&byte, l)) {
					for(j = 0; j < (freq / (double)turbo->oneL); j++) b2bset(tape, (*len)++, koef);
					koef = 1 - koef;
					for(j = 0; j < (freq / (double)turbo->oneL); j++) b2bset(tape, (*len)++, koef);
					koef = 1 - koef;
				} else {
					for(j = 0; j < (freq / (double)turbo->zeroL); j++) b2bset(tape, (*len)++, koef);
					koef = 1 - koef;
					for(j = 0; j < (freq / (double)turbo->zeroL); j++) b2bset(tape, (*len)++, koef);
					koef = 1 - koef;
				}
			}
			for(j = 0; j < ((double)44100 * (double)turbo->pause / (double)1000); j++) b2bset(tape, (*len)++, koef);
			if(turbo->pause) koef = 1 - koef;
			return true;
		}
		case TBLK_STD_SPEED: {
			auto rom = (STD_BLOCK*)blk->info;
			for(i = 0; i < 8064; i++) {
				for(j = 0; j < (freq / (double)2168); j++) b2bset(tape, (*len)++, koef);
				koef = 1 - koef;
				for(j = 0; j < (freq / (double)2168); j++) b2bset(tape, (*len)++, koef);
				koef = 1 - koef;
			}
			for(j = 0; j < (freq / (double)667); j++) b2bset(tape, (*len)++, koef);
			koef = 1 - koef;
			for(j = 0; j < (freq / (double)735); j++) b2bset(tape, (*len)++, koef);
			koef = 1 - koef;
			for(i = 0; i < (rom->len); i++) {
				byte = blk->data[i];
				for(l = 0; l < 8; l++) {
					if(b2bget(&byte, l)) {
						for(j = 0; j < (freq / (double)1710); j++) b2bset(tape, (*len)++, koef);
						koef = 1 - koef;
						for(j = 0; j < (freq / (double)1710); j++) b2bset(tape, (*len)++, koef);
						koef = 1 - koef;
					} else {
						for(j = 0; j < (freq / (double)855); j++) b2bset(tape, (*len)++, koef);
						koef = 1 - koef;
						for(j = 0; j < (freq / (double)855); j++) b2bset(tape, (*len)++, koef);
						koef = 1 - koef;
					}
				}
			}
			for(j = 0; j < (44100.0 * (double)rom->pause / 1000.0); j++) b2bset(tape, (*len)++, koef);
			koef = 1 - koef;
			return true;
		}
		case TBLK_PAUSE_OR_STOP: {
			memcpy(&len16, blk->info, 2);
			if(!len16) return false;
			koef = 0;
			for(j = 0; j < (44100.0 * (double)len16 / 1000.0); j++) b2bset(tape, (*len)++, koef);
			koef = 1 - koef;
			return true;
		}
		case TBLK_PURE_TONE: {
			auto tone = (PURE_TONE*)blk->info;
			for(i = 0; i < tone->n; i++) {
				for(j = 0; j < (freq / (double)tone->len); j++) b2bset(tape, (*len)++, koef);
				koef = 1 - koef;
			}
			return true;
		}
		case TBLK_PULSE_SEQ: {
			auto tlen = blk->len / 2;
			auto pulses = (ssh_w*)blk->data;
			for(i = 0; i < tlen; i++) {
				for(j = 0; j < (freq / (double)pulses[i]); j++) b2bset(tape, (*len)++, koef);
				koef = 1 - koef;
			}
			return true;
		}
		case TBLK_PURE_DATA: {
			auto data = (PURE_DATA*)blk->info;
			for(i = 0; i < (unsigned int)(data->l1 + data->l2 * 256 + data->l3 * 65536 - 1); i++) {
				byte = blk->data[i];
				for(l = 0; l < 8; l++) {
					if(b2bget(&byte, l)) {
						for(j = 0; j < (freq / (double)data->o); j++) b2bset(tape, (*len)++, koef);
						koef = 1 - koef;
						for(j = 0; j < (freq / (double)data->o); j++) b2bset(tape, (*len)++, koef);
						koef = 1 - koef;
					} else {
						for(j = 0; j < (freq / (double)data->z) - (((data->z) < 700) ? 2 : 3); j++) b2bset(tape, (*len)++, koef);
						koef = 1 - koef;
						for(j = 0; j < (freq / (double)data->z) - (((data->z) < 700) ? 2 : 3); j++) b2bset(tape, (*len)++, koef);
						koef = 1 - koef;
					}
				}
			}
			byte = blk->data[i];
			for(l = 0; l < data->used; l++) {
				if(b2bget(&byte, l)) {
					for(j = 0; j < (freq / (double)data->o); j++) b2bset(tape, (*len)++, koef);
					koef = 1 - koef;
					for(j = 0; j < (freq / (double)data->o); j++) b2bset(tape, (*len)++, koef);
					koef = 1 - koef;
				} else {
					for(j = 0; j < (freq / (double)data->z) - (((data->z) < 700) ? 2 : 3); j++) b2bset(tape, (*len)++, koef);
					koef = 1 - koef;
					for(j = 0; j < (freq / (double)data->z) - (((data->z) < 700) ? 2 : 3); j++) b2bset(tape, (*len)++, koef);
					koef = 1 - koef;
				}
			}
			for(j = 0; j < (44100.0 * (double)data->pause / 1000.0); j++) b2bset(tape, (*len)++, koef);
			if(data->pause) koef = 1 - koef;
			return true;
		}
		case TBLK_DIRECT_REC: {
			auto direct = (DIRECT_REC*)blk->info;
			if(direct->TS == 79) {
				for(i = 0; i < (blk->len * 8 - 1); i++) b2bset(tape, (*len)++, b2bget(blk->data, i));
				for(l = 0; l < direct->used; l++) b2bset(tape, (*len)++, b2bget(blk->data, i + l));
			} else {
				for(i = 0; i < (blk->len * 8 - 1); i++) {
					b2bset(tape, (*len)++, b2bget(blk->data, i));
					b2bset(tape, (*len)++, b2bget(blk->data, i));
				}
				for(l = 0; l < direct->used; l++) {
					b2bset(tape, (*len)++, b2bget(blk->data, i + l));
					b2bset(tape, (*len)++, b2bget(blk->data, i + l));
				}
			}
			for(j = 0; j < ((double)44100 * (double)direct->pause / (double)1000); j++) b2bset(tape, (*len)++, koef);
			if(direct->pause) koef = 1 - koef;
			return true;
		}
		case TBLK_GROUP_END: {
			for(j = 0; j < (freq / 4000.0); j++) b2bset(tape, (*len)++, koef);
			koef = 1 - koef;
			return true;
		}
	}
	return false;
}

int v44100_22050 = 0;
void zxTape::update(int ticks) {
	rewCounter -= ticks;
	if(rewCounter < -2) {
		rewCounter = rewCounterConst;
		if(!isDirectMic) {
			if(v44100_22050) {
				tapeMIC[micPos++] = out;
				if(micPos >= micMax) {
//					WorkWithBuffer2(tapeMIC, micPos);
					micPos = 0;
					micPrevPos = 0;
				}
			}
			v44100_22050 = 1 - v44100_22050;
		} else {
			b2bset(micBuf, micPos, (out < 128 ? 0 : 1));
			micPos++;
			if(micPos >= micMaxLen * 8) {
				micPos--;
				addMicBlock();
			}
		}
		if(isRecording) {
			if(recPosBuffer == (DIRECT_REC_SIZE - 1)) {
//				io_read(audio3, recBuffer, DIRECT_RECORDING_BUFFER_SIZE);
				recPosBuffer = 0;
				return;
			}
			recPosBuffer++;
			return;
		}
		if(pos >= len || isPause) return;
		pos++;
//		AYio_writeRegNV(BEEPER, b2bget(tape, pos));
		if(pos == len) {
			if(!next()) control(0); else resetBlock();
		}
	}
}

void zxTape::control(int force) {
	ssh_w len16;
	ssh_w rep = 0;
	int group = 0;
	auto loop = curr;

	if(!curr) return;

	len = pos = 0;

	if(isSkip()) next();
	if(isStd() && !force) { auto blk = curr->type; neproigrannaya_pausa = ((STD_BLOCK*)blk->info)->pause; return; }

//	Zamolchi();

	koef = 0;
	for(ssh_i j = 0; j < (44100.0 * (double)neproigrannaya_pausa / 1000.0); j++) b2bset(tape, len++, koef);
	neproigrannaya_pausa = 0;

	koef = 0;
	auto node = curr;
	while(node) {
		auto blk = node->type;
		auto id = blk->id;
		if(id == TBLK_PAUSE_OR_STOP) {
			memcpy(&len16, blk->info, 2);
			if(!len16) { pause(); break; } else fill(tape, &len, blk);
		}
		if(fill(tape, &len, blk)) {
			if(id == TBLK_GROUP_START) group = 1;
			else if(id == TBLK_GROUP_END) group = 0;
			else if(id == TBLK_LOOP_START) { rep = *(ssh_w*)blk->info; loop = node->next; }
			else if(id == TBLK_LOOP_END) { if(rep) rep--; if(rep) { node = loop; continue; } }
			else if(blk->id == TBLK_JUMP) {
				auto jump = *(short*)blk->info;
				while(jump != 0) {
					if(jump < 0) { jump++; node = blocks.parent(node); } else { jump--; node = node->next; }
				}
				curr = node;
				control(0);
				break;
			}
			if(!group && !rep) { rewCounter = rewCounterConst; break; }
			node = node->next;
		}
	}
}

bool zxTape::loadTAP(ssh_cws path) {
	ssh_w len;
	STD_BLOCK blk;

	zxFile f;

	if(!f.open(path, true)) return false;

	reset();

	while(true) {
		if(!f.read(&len, 2) || len < 2) break;
		if(!f.read(TMP_BUF, len)) { reset(); return false; }
		if(len) {
			blk.len = len;
			blk.pause = 1000;
			addBlock(TBLK_STD_SPEED, len, TMP_BUF, sizeof(STD_BLOCK), &blk);
		}
	}
	resetBlock();
	return true;
}

bool zxTape::loadTZX(ssh_cws path) {
	static STD_BLOCK std;
	static TURBO_BLOCK turbo;
	static PURE_TONE tone;
	static PURE_DATA pure;
	static DIRECT_REC rec;

	int len;
	ssh_b id;

	zxFile f;

	if(!f.open(path, true)) return false;

	reset();

	while(true) {
		if(!f.read(&id, 1)) { resetBlock(); return true; }
		switch(id) {
			case TBLK_TEXT_DESC:
				len = 0;
				if(!f.read(&len, 1)) break;
				if(!f.read(TMP_BUF, len)) break;
				addBlock(id, len, TMP_BUF, 0, nullptr);
				break;
			case TBLK_STD_SPEED:
				if(!f.read(&std, sizeof(STD_BLOCK))) break;
				if(!f.read(TMP_BUF, std.len)) break;
				addBlock(id, std.len, TMP_BUF, sizeof(STD_BLOCK), &std);
				break;
			case TBLK_TURBO_SPEED:
				if(!f.read(&turbo, sizeof(TURBO_BLOCK))) break;
				len = turbo.l1 + turbo.l2 * 256 + turbo.l3 * 65536;
				if(len > 131072) {
					MessageBox(GetDesktopWindow(), L"Turbo block > 131072 bytes", L"ZX", MB_ICONWARNING);
					break;
				}
				if(!f.read(TMP_BUF, len)) break;
				addBlock(id, len, TMP_BUF, sizeof(TURBO_BLOCK), &turbo);
				break;
			case TBLK_ARCHIVE_INFO:
				len = 0;
				if(!f.read(&len, 2)) break;
				if(!f.read(TMP_BUF, len)) break;
				addBlock(id, len, TMP_BUF, 0, nullptr);
				break;
			case TBLK_GROUP_START:
				len = 0;
				if(!f.read(&len, 2)) break;
				if(!f.read(TMP_BUF, len)) break;
				addBlock(id, len, TMP_BUF, 0, nullptr);
				break;
			case TBLK_GROUP_END:
				addBlock(id, 0, nullptr, 0, nullptr);
				break;
			case TBLK_PURE_TONE:
				if(!f.read(&tone, sizeof(PURE_TONE))) break;
				addBlock(id, 0, nullptr, sizeof(PURE_TONE), &tone);
				break;
			case TBLK_PULSE_SEQ:
				len = 0;
				if(!f.read(&len, 2)) break;
				len *= 2;
				if(!f.read(TMP_BUF, len)) break;
				addBlock(id, len, TMP_BUF, 0, nullptr);
				break;
			case TBLK_PURE_DATA:
				if(!f.read(&pure, sizeof(PURE_DATA))) break;
				len = pure.l1 + pure.l2 * 256 + pure.l3 * 65536;
				if(len > 131072) {
					MessageBox(GetDesktopWindow(), L"Pure data > 131072 bytes", L"ZX", MB_ICONWARNING);
					break;
				}
				if(!f.read(TMP_BUF, len)) break;
				addBlock(id, len, TMP_BUF, sizeof(PURE_DATA), &pure);
				break;
			case TBLK_LOOP_START:
				len = 0;
				if(!f.read(&len, 2)) break;
				addBlock(id, 0, nullptr, 2, &len);
				break;
			case TBLK_LOOP_END:
				addBlock( id, 0, nullptr, 0, nullptr);
				break;
			case TBLK_PAUSE_OR_STOP:
				len = 0;
				if(!f.read(&len, 2)) break;
				addBlock(id, 0, nullptr, 2, &len);
				break;
			case TBLK_JUMP:
				len = 0;
				if(!f.read(&len, 2)) break;
				addBlock(id, 0, nullptr, 2, &len);
				break;
			case TBLK_CUSTOM_INFO:
				if(!f.read(TMP_BUF, 16)) break;
				if(!f.read(&len, 4)) break;
				if(!f.read(&TMP_BUF[16], len)) break;
				addBlock(id, len, &TMP_BUF[16], 16, TMP_BUF);
				break;
			case TBLK_HARDWATE_TYPE:
				len = 0;
				if(!f.read(&len, 1)) break;
				len *= 3;
				if(!f.read(TMP_BUF, len)) break;
				addBlock(id, len * 3, TMP_BUF, 0, nullptr);
				break;
			case TBLK_VERSION:
				f.setPos(7, SEEK_CUR);
				if(!f.read(TMP_BUF, 2)) break;
				addBlock(id, 0, nullptr, 2, TMP_BUF);
				break;
			case TBLK_DIRECT_REC:
				if(!f.read(&rec, sizeof(DIRECT_REC))) break;
				len = rec.l1 + rec.l2 * 256 + rec.l3 * 65536;
				if(len > 131072) {
					MessageBox(GetDesktopWindow(), L"Direct record > 131072 bytes", L"ZX", MB_ICONWARNING);
					break;
				}
				if(rec.TS != 158 && rec.TS != 79) {
					MessageBox(GetDesktopWindow(), L"Direct record number of T states per sample != 158 and 79!", L"ZX", MB_ICONWARNING);
					break;
				}
				if(!f.read(TMP_BUF, len)) break;
				addBlock(id, len, TMP_BUF, sizeof(DIRECT_REC), &rec);
				break;
		}
	}
	reset();

	return false;
}

bool zxTape::saveTAP(ssh_cws path) {
	zxFile f;

	if(!f.open(path, false)) return false;
	
	auto node = blocks.root();
	while(node) {
		auto blk = node->type;
		if(blk->id == TBLK_STD_SPEED) {
			auto len = blk->len;
			f.write(&len, 2);
			f.write(blk->data, len);
		}
		node = node->next;
	}

	return true;
}

bool zxTape::saveTZX(ssh_cws path) {
	ssh_b eof = 26;
	int l;

	auto node = blocks.root();
	if(!node) return false;
	zxFile f;

	if(!f.open(path, false)) return false;

	ssh_b major = 1, minor = 13;
	if(!f.write("ZXTape!", 7)) return false;
	if(!f.write(&eof, 1)) return false;
	if(!f.write(&major, 1)) return false;
	if(!f.write(&minor, 1)) return false;

	while(node) {
		auto blk = node->type;
		if(!f.write(&blk->id, 1)) return false;
		auto info = blk->info;
		auto data = blk->data;
		auto len = blk->len;
		switch(blk->id) {
			case TBLK_PULSE_SEQ:
				l = len / 2;
				if(!f.write(&l, 1)) return false;
				break;
			case TBLK_GROUP_START:
				if(!f.write(&len, 1)) return false;
				break;
			case TBLK_TEXT_DESC:
				if(!f.write(&len, 1)) return false;
				break;
			case TBLK_ARCHIVE_INFO:
				if(!f.write(&len, 2)) return false;
				break;
			case TBLK_HARDWATE_TYPE:
				l = len / 3;
				if(!f.write(&l, 1)) return false;
				break;
			case TBLK_CUSTOM_INFO:
				if(!f.write(info, 16)) return false;
				if(!f.write(&len, 4)) return false;
				info = nullptr;
				break;
		}
		if(info) { if(!f.write(info, blk->ilen)) return false; }
		if(data) { if(!f.write(data, len)) return false; }
		node = node->next;
	}
	return true;
}

bool zxTape::loadWAV(ssh_cws path) {
	/*
	IDWave id;
	int fil;
	unsigned int len32;

	signed short data16;
	unsigned char data8;
	unsigned int i;
	unsigned char * data;
	unsigned int true_len;

	signed short v161, v162;
	unsigned char v81,  v82;

	unsigned char bit;

	unsigned int counter=0;

	direct_recT direct;

	unsigned char used=0;


	FreeTape();

	fil=io_open(NULL, filename, O_RDONLY);

	if (fil==-1)
	{
		sprintf(tstring,"io_readWAV : %s - can't io_open file", filename);
		MessageBoxErr(tstring);
		return;
	}

	io_lseek(fil, 0x14, SEEK_SET);
	io_read(fil, (unsigned char *)&id.type, sizeof(id.type));
	io_read(fil, (unsigned char *)&id.channels, sizeof(id.channels));
	io_read(fil, (unsigned char *)&id.SamplesPerSec, sizeof(id.SamplesPerSec));
	io_lseek(fil, 6, SEEK_CUR);
	io_read(fil, (unsigned char *)&id.bits, sizeof(id.bits));

#if defined BIGENDIAN
byteswp((unsigned char *)&id.type);
byteswp((unsigned char *)&id.channels);
ByteSWP_32(&id.SamplesPerSec);
byteswp((unsigned char *)&id.bits);
#endif


	if (id.type!=1)
	{
		MessageBoxErr("io_readWAV : Only PCM files supported !");
		io_close(fil);
		return;
	}

	if ((id.bits!=8)&&(id.bits!=16))
	{
		sprintf(tstring,"io_readWAV : bits = %d unsupported. Only 8 and 16 bit files supported !", id.bits);

		MessageBoxErr(tstring);
		io_close(fil);
		return;
	}

	if ((id.channels!=2)&&(id.channels!=1))
	{
		MessageBoxErr("io_readWAV : Only MONO and STEREO files supported !");
		io_close(fil);
		return;
	}

	if ((id.SamplesPerSec!=11025)&&(id.SamplesPerSec!=44100)&&(id.SamplesPerSec!=22050))
	{
		sprintf(tstring, "SamplesPerSec=%d unsupported. Supported SamplesPerSec are : 11025, 22050, 44100", id.SamplesPerSec);

		MessageBoxErr(tstring);
		io_close(fil);
		return;
	}

	io_lseek(fil, 4, SEEK_CUR);
	io_read(fil, (unsigned char *)&len32, sizeof(len32));

#if defined BIGENDIAN
ByteSWP_32(&len32);
#endif

	true_len=len32;
	if (id.SamplesPerSec==11025) true_len*=2;
	if (id.bits==16) { true_len/=2; len32/=2; }
	if (id.channels==2) { true_len/=2; len32/=2; }

	true_len/=8;
	true_len++;


	data=(unsigned char *)malloc(true_len);
	if (data==NULL)
	{
		MessageBoxErr("io_readWAV : malloc failed. This is bug. Report it.");
		return;
	}

	for (i=0; i<len32; i++)
	{
		if (id.channels==1)
		{
			if (id.bits==16)
			{
			io_read (fil, (unsigned char *)&data16, 2);
#if defined BIGENDIAN
byteswp((unsigned char *)&data16);
#endif
			}
			else io_read (fil, (unsigned char *)&data8, 1);
		}
		else
		{
			if (id.bits==16)
			{
				io_read (fil, (unsigned char *)&v161, 2);
				io_read (fil, (unsigned char *)&v162, 2);

#if defined BIGENDIAN
byteswp((unsigned char *)&v161);
byteswp((unsigned char *)&v162);
#endif

				data16=v161/2+v162/2;
			}
			else
			{
				io_read (fil, (unsigned char *)&v81, 1);
				io_read (fil, (unsigned char *)&v82, 1);
				data8=v81/2+v82/2;
			}
		}

		if (id.bits==8)
		{
			if (data8>127) bit=1;
			else 				bit=0;
		}
		else
		{
			if (data16&0x8000) bit=0;
			else 			  		 bit=1;
		}

		if (id.SamplesPerSec==11025)

		{
			b2bset(data, counter++, bit);
			used++; if (used==9) used=1;
			b2bset(data, counter++, bit);
			used++; if (used==9) used=1;
		}
		else
		{
			b2bset(data, counter++, bit);
			used++; if (used==9) used=1;
		}

		if (((i/1000000*1000000)==i)||(!i))
			printf("io_reading %s. %d%c done...\n", filename, i*100/len32, '%');

	}

	printf("io_reading %s. 100%c done.\n", filename, '%');

	if (id.SamplesPerSec==44100) direct.TS=79;
	else 								  direct.TS=158;

	direct.pause=0;
	direct.used=used;
	direct.l3=(true_len&0xFF0000)/65536;
	direct.l2=(true_len&0x00FF00)/256;
	direct.l1= true_len&0x0000FF;

	AddTapeBlock("", 0x15, true_len, data, sizeof(direct), &direct, 1);

	io_close(fil);
	tape_type="wav";
	sprintf(tape_filename,filename);
	tape_SetActiveAndPause(tapes);
	return;	*/
return false;
}

bool zxTape::saveWAV(ssh_cws path) {
	/*
	int fil;
	tape_struct * t;
	unsigned int len=0;
	unsigned int full_len;
	unsigned int temp;
	unsigned short temp16;
	unsigned int i;
	unsigned char byte;
	unsigned char Data=0;


	if (tapes==NULL) return;

	fil=io_creat(NULL, filename);

	if(fil==-1)
	{
		sprintf(tstring,"SaveWAV : Can't create %s. You must be able to io_write this directory.", filename);
		MessageBoxErr(tstring);
		return;
	}


	t=tapes;

	while (TRUE)
	{
		Fill(tapeS, &len, t);

		t= (tape_struct *)t->next;
		if (!t) break;
	}

	full_len=len/*dlina viborki+8/*zagolovok RIFF fajla + 12/*Zagolovok kuska WAV + 16/*kusok WAV + 8/*identifikator vyborki;

	io_write(fil, "RIFF", 4);
	temp = full_len - 8;
	io_write(fil, (unsigned char*)& temp, 4);
	io_write(fil, "WAVEfmt ", 8);
	temp = 0x10;
	io_write(fil, (unsigned char*)& temp, 4);
	temp16 = 1;
	io_write(fil, (unsigned char*)& temp16, 2);
	io_write(fil, (unsigned char*)& temp16, 2);
	temp = 44100;
	io_write(fil, (unsigned char*)& temp, 4);
	io_write(fil, (unsigned char*)& temp, 4);
	io_write(fil, (unsigned char*)& temp16, 2);
	temp16 = 8;
	io_write(fil, (unsigned char*)& temp16, 2);
	io_write(fil, "data", 4);
	temp = len;
	io_write(fil, (unsigned char*)& temp, 4);
	io_write(fil, (unsigned char*)& Data, 1);

	for(i = 0; i < len; i++) {
		byte = b2bget(tapeS, i);
		if(byte) byte = 250;

		io_write(fil, &byte, 1);

		if(((i / 1000000 * 1000000) == i) || (!i))
			printf("Writing %s. %d%c done...\n", filename, i * 100 / len, '%');
	}

	printf("Writing %s. 100%c done.\n", filename, '%');
	io_close(fil);
	*/
	return false;
}
