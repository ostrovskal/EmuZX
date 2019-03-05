
#include "stdafx.h"
#include "zxBus.h"
#include "zxCPU.h"
#include "zxGPU.h"
#include "zxSND.h"
#include "zxDebugger.h"
#include "zxKeyboard.h"

extern ssh_cws nameROMs[];

#pragma pack(push, 1)
struct HEAD1_Z80 {
	ssh_w AF, BC, HL, PC, SP;
	ssh_b I, R;
	ssh_b STATE1;				// 0 - 7 бит R, 1-3 - бордер
	ssh_w DE;
	ssh_w BC_, DE_, HL_, AF_;
	ssh_w IY, IX;
	ssh_b IFF1, IFF2;
	ssh_b STATE2;				// 0-1 IM
};

struct HEAD2_Z80 {
	HEAD1_Z80 head1;

	ssh_w length;				// длина HEAD2 + HEAD3 + если экстра байт для скорпиона
	ssh_w PC;					// новый ПС
	ssh_b hardMode;				// тип оборудования
	ssh_b hardState;			// значение порта 7ffd для 128К
	ssh_b interState;			// 255 - ROM1?
	ssh_b emulateFlags;			// 7 - признак мрдифицированного оборудования
	ssh_b sndChipRegNumber;		// номер последнего AY регистра
	ssh_b sndRegs[16];			// содержимое всех AY регистров
};

struct HEAD3_Z80 {
	HEAD2_Z80 head2;

	ssh_w lowTSTATE;			// не знаю зачем он нужен
	ssh_b highTSTATE;			// аналогично
	ssh_b reserved;				// резерв
	ssh_b mgtRomPaged;			// принтер - мне не надо
	ssh_b multifaceRomPaged;	// мне не надо
	ssh_b mem0000_1FFF;			// мне не надо
	ssh_b mem2000_3FFF;			// мне не надо
	ssh_b keyMaps[10];			// я сделал по своему
	ssh_b asciiVals[10];		// аналогично
	ssh_b mgtType;				// тип принтера
	ssh_b inhibitorButton;		// магическая кнопка - нахрена?
	ssh_b inhibitorFlag;		// аналогично
	ssh_b port1FFFD;			// значение порта 1ffd для скорпиона (типа версия 4)
	// блоки данных:
	// 2 - длина блока(65535 - не сжатый, иначе длина сжатой страницы)
	// 1 - номер страницы(48К : 4 - 2, 5 - 0, 8 - 5, 128К и пентагон : 3-10 - 0-7, скорпион: 3-18 - 0-15)
	// типы оборудования:
	// 48К - 2(0,1) 3(0,1) 128К - все кроме(3(3)),12,13, пентагон - 9, скорпион - 10
};
#pragma pack(pop)

ssh_b* TR_DOS = nullptr;

// размер ПЗУ
long szROM = 0;

// адрес возврата из процедуры
ssh_w RET_CALL;

// актуальные страницы
ssh_b* pageROM(nullptr);
ssh_b* pageRAM(nullptr);
ssh_b* pageVRAM(nullptr);
ssh_b* pageATTRIB(nullptr);

// страницы озу
ssh_b* PAGE_RAM[64];

// страницы пзу
ssh_b* PAGE_ROM[8];

// регистры
ssh_b cpuZX[COUNT_REGS];

// счетчик тактов процессора
ssh_d* _TICK((ssh_d*)&cpuZX[TICK0]);

ssh_b* _FE16(&cpuZX[FE16]);
ssh_b* _FE8(&cpuZX[FE8]);

// массив из 16 регистров AY
ssh_b* _AY(&cpuZX[AY0]);

// адрес прерывания
ssh_b* _I(&cpuZX[RI]);

// ренегерация
ssh_b* _R(&cpuZX[RR]);

// режим прерываний
ssh_b* _IM(&cpuZX[IM]);

// триггеры
ssh_b* _IFF1(&cpuZX[IFF1]);
ssh_b* _IFF2(&cpuZX[IFF2]);

// содержимое портов
ssh_b* _FE(&cpuZX[PORT_FE]);
ssh_b* _1FFD(&cpuZX[PORT_1F]);
ssh_b* _7FFD(&cpuZX[PORT_FD]);
ssh_b* _KEYS(&cpuZX[KEYS0]);
ssh_b* _KEMPSTON(&cpuZX[KEMPSTON]);
ssh_b* _AY_REG(&cpuZX[AY_REG]);
ssh_b* _MOUSE(&cpuZX[MOUSE0]);
ssh_b* _TRDOS(&cpuZX[TRDOS0]);

// RAM
ssh_b* _RAM(&cpuZX[RAM]);
// VID
ssh_b* _VID(&cpuZX[VID]);
// ROM
ssh_b* _ROM(&cpuZX[ROM]);

// текущая модель памяти
ssh_b* _MODEL(&cpuZX[MODEL]);

// регистры общего назначения
ssh_b* _A(&cpuZX[RA]);
ssh_b* _B(&cpuZX[RB]);

ssh_w* _BC((ssh_w*)&cpuZX[RC]);
ssh_w* _DE((ssh_w*)&cpuZX[RE]);
ssh_w* _HL((ssh_w*)&cpuZX[RL]);
ssh_w* _AF((ssh_w*)&cpuZX[RF]);
ssh_w* _SP((ssh_w*)&cpuZX[RSPL]);
ssh_w* _IX((ssh_w*)&cpuZX[RXL]);
ssh_w* _IY((ssh_w*)&cpuZX[RYL]);
ssh_w* _PC((ssh_w*)&cpuZX[RPCL]);
// статус
ssh_w* _TSTATE((ssh_w*)&cpuZX[TSTATE1]);

zxBus::zxBus() : ROMs(nullptr), RAMs(nullptr), isSound(false), isJoystick(false), periodGPU(0), nRetPort(255), border(7) {
	makeTblParity();
	TR_DOS = new ssh_b[16384];
}

zxBus::~zxBus() {
	SAFE_A_DELETE(ROMs);
	SAFE_A_DELETE(RAMs);
	SAFE_A_DELETE(TR_DOS);
}

ssh_d zxBus::updateStates() {
	static int modelParams[] = {
		8960, 8, 56, 7168, // 69888,// 16192
		8892, 8, 60, 7296, // 70908,// 16256
		12544,52,12, 5376, // 71680,// 17984
		8960, 48,16, 7168, // 69888 // 16192
	};

	auto params = &modelParams[*_MODEL * 4];
	int turbo = (theApp->getOpt(OPT_TURBO)->dval ? 4 : 1);
	auto periodCPU = theApp->getOpt(OPT_PERIOD_CPU)->dval * 2;
	periodCPU *= turbo;
	
	stateUP = params[0] * periodCPU / 10;
	stateLP = params[1] * periodCPU / 10;
	stateRP = params[2] * periodCPU / 10;
	stateDP = params[3] * periodCPU / 10;

	return periodCPU;
}

void zxBus::updateData() {
	// наличие звука, джойстика
	isSound = theApp->getOpt(OPT_SOUND)->dval;
	isJoystick = theApp->getOpt(OPT_JOYSTICK)->dval;
	// обновить скорость обновления кадра
	auto period = updateStates();
	// обновить ГПУ
	GPU.updateData();
	// громкость звука
	SND.beeperVolume = theApp->getOpt(OPT_SND_BEEPER_VOL)->dval;
	// обновить звук
	SND.setTicksPerFrame(period);
}

bool zxBus::changeModel(ssh_d newModel, ssh_d oldModel) {
	if(newModel != oldModel) {
		// грузим ПЗУ
		zxFile f;
		// загрузить TR DOS
		if(f.open(theApp->opts.mainDir + L"Roms\\TR_DOS.rom", true)) {
			f.read(TR_DOS, 16384);
			f.close();
		}
		ssh_b* pROM = nullptr;
		zxString pathROM(theApp->opts.mainDir + L"Roms\\" + nameROMs[newModel] + L".rom");
		if(f.open(pathROM, true)) {
			szROM = f.length();
			pROM = new ssh_b[szROM];
			if(!f.read(pROM, szROM)) {
				// не удалось загрузить ПЗУ
				delete[] pROM;
				pROM = nullptr;
			}
		}
		if(pROM) {
			int i;
			// имя загруженной проги по умолчанию
			theApp->getOpt(OPT_MODEL)->dval = newModel;
			*_MODEL = (ssh_b)newModel;
			SAFE_A_DELETE(ROMs);
			SAFE_A_DELETE(RAMs);
			ROMs = pROM;
			// устанавливаем параметры
			updateStates();
			auto pagesRAM = countPages(false);
			auto pagesROM = countPages(true);
			RAMs = new ssh_b[pagesRAM * 16384];
			memset(PAGE_RAM, 0, sizeof(PAGE_RAM));
			memset(PAGE_ROM, 0, sizeof(PAGE_ROM));
			for (i = 0; i < pagesRAM; i++) PAGE_RAM[i] = (RAMs + i * 16384);
			for (i = 0; i < pagesROM; i++) PAGE_ROM[i] = (ROMs + i * 16384);
		}
	}
	signalRESET();
	return true;
}

void zxBus::signalRESET() {
	SND.stop();
	TAPE.reset();
	//DISK.reset();
	memset(RAMs, 0, countPages(false) * 16384);
	memset(cpuZX, 0, VID);
	memset(_KEYS, 255, 8);
	*_SP = 65534;
	*_FE = 0b11100111;
	RET_CALL = 0;
	*_VID = 5;
	setPages();
	modifyTSTATE(0, ~(ZX_EXEC | ZX_DEBUG));
	theApp->opts.nameLoadProg = L"BASIC";
	theApp->updateCaption();
}

bool zxBus::loadState(ssh_cws path) {
	ssh_b tmp[COUNT_REGS];
	ssh_d size;
	zxFile f;

	if(!f.open(path, true)) return true;
	auto l = f.length();
	// грузим регистры
	if(!f.read(tmp, COUNT_REGS)) return false;
	// меняем модель
	if(!changeModel(*_MODEL, tmp[MODEL])) return false;
	// грузим регистры
	memcpy(cpuZX, tmp, COUNT_REGS);
	// грузим банки ОЗУ
	int banks = countPages(false);
	for(int i = 0; i < banks; i++) {
		if(!f.read(&size, 4)) return false;
		if(!f.read(TMP_BUF, size)) return false;
		if(!unpackBlock(TMP_BUF, PAGE_RAM[i], PAGE_RAM[i] + 16384, size, true, false)) return false;
	}
	// восстанавливаем страницы
	setPages();
	// грузим имя сохраненной проги
	l -= f.getPos();
	if(!f.read(TMP_BUF, l)) return false;
	theApp->opts.nameLoadProg = (ssh_cws)TMP_BUF;
	return true;
}

bool zxBus::saveState(ssh_cws path) {
	ssh_b pageTMP[18000];
	ssh_d size;

	zxFile f(path, false);
	// сохраняем регистры
	if(!f.write(cpuZX, COUNT_REGS)) return false;
	// количество банков и их содержимое
	int banks = countPages(false);
	for(int i = 0; i < banks; i++) {
		auto block = packBlock(PAGE_RAM[i], PAGE_RAM[i] + 16384, pageTMP, false, size);
		if(!f.write(&size, 4)) return false;
		if(!f.write(block, size)) return false;
	}
	// имя загруженной проги
	zxString* progs(&theApp->opts.nameLoadProg);
	auto l = (int)(progs->length() + 1) * 2;
	if(!f.write(progs->buffer(), l)) return false;
	return true;
}

void zxBus::setPages() {
	pageROM = ((*_TSTATE & ZX_TR_DOS) ? TR_DOS : (*_ROM == 100 ? PAGE_RAM[0] : PAGE_ROM[*_ROM]));
	pageRAM = PAGE_RAM[*_RAM];
	pageVRAM = PAGE_RAM[*_VID];
	pageATTRIB = pageVRAM + 6144;
}

int zxBus::signalINT() {
	if(*_IFF1) {
		modifyTSTATE(0, ZX_HALT);
		*_IFF1 = *_IFF2 = 0;
		CPU.incrementR();
		switch(*_IM) {
			case 0: case 1: CPU.execCALL(0x38); return 13;
			case 2: CPU.execCALL(read_mem16((*_I) * 256 + 254)); return 19;
		}
	}
	return 0;
}

int zxBus::signalNMI() {
	*_IFF1 = 0;
	CPU.execCALL(0x66);
	return 11;
}

int zxBus::step(bool allow_int) {
	if((*_TSTATE) & ZX_EXEC) {
		modifyTSTATE(allow_int * ZX_INT, ZX_TR_DOS);
		if(*_TSTATE & ZX_DEBUG) {
			if(theApp->debug->checkBPS(*_PC, false)) return 0;
		}
		// перехват системных процедур
		TAPE.hookPC(*_PC);
		// выполнение операции
		int ticks = CPU.execOps(0, 0);
		// проверка на прерывания
		if((*_TSTATE) & ZX_NMI) ticks += signalNMI();
		else if((*_TSTATE) & ZX_INT) ticks += signalINT();
		(*_TICK) += ticks;
		return ticks;
	}
	return 0;
}

void zxBus::stepDebug() {
	modifyTSTATE(0, ZX_HALT | ZX_TR_DOS);
	// перехват системных процедур
	TAPE.hookPC(*_PC);
	// выполнение операции
	(*_TICK) += CPU.execOps(0, 0);
}

void zxBus::execute() {
	// опрос процессора, отображение экрана, воспроизведение звука, опрос джойстика
	if((*_TSTATE) & ZX_RESET) signalRESET();
	if(*_TSTATE & ZX_EXEC) {
		if(isJoystick) theApp->keyboard->processJoystick();
		GPU.updateFrame();
		if(isSound) {
			//SND.execute();
			//SND.play();
		}
	}
}

ssh_b zxBus::readPort(ssh_b A0A7, ssh_b A8A15) {
	// TR DOS
	if(*_TSTATE & ZX_TR_DOS) return DISK.readPort(A0A7);
	// ALL
	switch(A0A7) {
		// звук AY
		case 0xfd:
			if(A8A15 == 0xff) return SND.ayRegs[*_AY_REG];
			break;
		// мышь
		case 0xdf:
			switch(A8A15) {
				case 0xfb: _MOUSE[0]; break; // x
				case 0xff: _MOUSE[1]; break; // y
				case 0xfa: _MOUSE[2]; break; // keys
			}
			break;
		// джойстик
		case 0x1f: return *_KEMPSTON;
		// клавиатура
		case 0xfe: {
			ssh_b ret = 255;
			for(int i = 0; i < 8; i++) {
				if(A8A15 & (1 << i)) continue;
				ret &= _KEYS[i];
			}
			return ret;
		}
	}
	return nRetPort;
}

void zxBus::writePort(ssh_b A0A7, ssh_b A8A15, ssh_b val) {
	// TR DOS
	if(*_TSTATE & ZX_TR_DOS) return DISK.writePort(A0A7, val);
	// ALL
	switch(A0A7) {
		// kempston
		case 0x1f: *_KEMPSTON = val; break;
		// covox
		//case 0xfb: break;
		// specdrum
		//case 0xdf: break;
		// 0, 1, 2 - бордер, 3 MIC - при записи/загрузке, 4 - бипер
		case 0xfe: {
			*_FE = val;
			border = val & 7;
			TAPE.out = (val & 0x8 ? 250 : 5);
			if(TAPE.isPause) {
				if((val & 16) != *_FE16) {
					*_FE16 = val & 16;
					SND.writeValue(SND_BEEPER, *_FE16);
				}
				if((val & 8) != *_FE8) {
					*_FE8 = val & 8;
					SND.writeValue(SND_BEEPER, *_FE8);
				}
			}
		}
		break;
		case 0xfd:
			switch(A8A15) {
				case 0xff: (*_AY_REG) = val & 0x0f; break;
				case 0xbf: if(TAPE.isPause) SND.writeAY(*_AY_REG, val); break;
				// SCORPION
				// 0 -> 1 - RAM0(!!), 0 - see bit 1 etc
				// 1 -> 1 - ROM 2, 0 - ROM from 0x7FFD
				// 4 -> 1 - RAM SCORPION, 0 - from 0x7FFD
				case 0x1f:
					if (*_MODEL != MODEL_SCORPION_256K) break;
					*_1FFD = val;
					if (val & 1) *_ROM = 100;
					else if (val & 2) *_ROM = 2;
					else *_ROM = ((*_7FFD) & 16) >> 4;
					*_RAM = ((*_7FFD) & 7) +((val & 16) >> 1);
					setPages();
					break;
				// 1, 2, 4 - страница 0-7
				// 8 - экран 5/7
				// 16 - ПЗУ 0 - 128К 1 - 48К
				// 32 - блокировка
				case 0x7f: {
					if((*_7FFD) & 32) break;
					*_7FFD = val;
					*_RAM = (val & 7) +(((*_1FFD) & 16) >> 1);
					*_VID = ((val & 8) ? 7 : 5);
					if(*_MODEL == MODEL_SCORPION_256K) {
						if(!((*_1FFD) & 3)) *_ROM = (val & 16) >> 4;
					} else {
						*_ROM = (val & 16) >> 4;
					}
					setPages();
				}
				break;
		}
	}
}

void zxBus::updateCPU(int todo, int interrupt) {
	// количество тактов на инструкцию
	int ticks;
	todo += periodGPU;
	if(interrupt) {
		if(periodGPU > 0) {
			ticks = step(false);
			todo -= ticks;
			TAPE.update(ticks);
		}
		ticks = step(true);
		todo -= ticks;
		TAPE.update(ticks);
	}
	while(todo > 3) {
		ticks = step(false);
		if(!ticks) {
			todo &= 3;
			break;
		}
		todo -= ticks;
		TAPE.update(ticks);
	}
	periodGPU = todo;
}

void zxBus::updateFrame(ssh_d* dest, ssh_d* colors, int sizeBorder, bool blink) {
	border = (*_FE) & 7;
	asm_ssh_update_frame(sizeBorder, blink, dest, colors, this, &zxBus::updateCPU, &pageVRAM);
}

bool zxBus::loadZ80(ssh_cws path) {
	ssh_b* ptr = nullptr;
	bool result = false;
	zxFile f;

	if(!f.open(path, true)) return false;

	try {
		auto sz = f.length();
		ptr = new ssh_b[sz];

		if(!f.read(ptr, sz)) throw(0);

		HEAD1_Z80* head1 = nullptr;
		HEAD2_Z80* head2 = nullptr;
		HEAD3_Z80* head3 = nullptr;
		ssh_d model = -1;

		int length = sizeof(HEAD1_Z80);
		head1 = (HEAD1_Z80*)ptr;
		auto buf = ptr;
		auto PC = head1->PC;
		if(PC == 0) {
			head2 = (HEAD2_Z80*)buf;
			bool isMod = (head2->emulateFlags & 128);
			if(head2->hardMode == 3 && !isMod) model = MODEL_128K;
			length += head2->length + 2;
			if(length > 55) {
				head3 = (HEAD3_Z80*)buf;
				if(head2->hardMode == 3 && !isMod) model = MODEL_48K;
				head2 = &head3->head2;
			}
			switch(head2->hardMode) {
				case 0: case 1: if(!isMod) model = MODEL_48K; break;
				case 4: case 5: case 6: if(!isMod) model = MODEL_128K; break;
				case 9: model = MODEL_PENTAGON_128K; break;
				case 10: model = MODEL_SCORPION_256K; break;
			}
			head1 = &head2->head1;
			PC = head2->PC;
		} else model = MODEL_48K;
		buf += length;
		sz -= length;
		if(model == -1) throw(0);
		if(!changeModel(model, *_MODEL)) throw(0);

		*_BC = head1->BC; *_DE = head1->DE; *_HL = head1->HL; *_AF = head1->AF;
		*_SP = head1->SP; *_IX = head1->IX; *_IY = head1->IY; *_PC = PC;
		*_I = head1->I; *_R = head1->R; *_IM = head1->STATE2 & 3; *_IFF1 = head1->IFF1; *_IFF2 = head1->IFF2;
		(*_R) |= (head1->STATE1 & 1) << 7;
		(*_FE) = (224 | ((head1->STATE1 & 14) >> 1));
		memcpy(&cpuZX[RC_], &head1->BC_, 8);

		if(head2) {
			while(sz > 0) {
				ssh_w szData = *(ssh_w*)buf;
				ssh_b* page = nullptr;
				int szSrc = (szData == 65535 ? 16384 : szData);
				int numPage = *(buf + 2);
				switch(model) {
					case MODEL_48K:
						switch(numPage) {
							case 4: page = PAGE_RAM[2]; break;
							case 5: page = PAGE_RAM[0]; break;
							case 8: page = PAGE_RAM[5]; break;
						}
						break;
					case MODEL_128K:
					case MODEL_PENTAGON_128K:
						if(numPage >= 3 && numPage <= 10) page = PAGE_RAM[numPage - 3];
						break;
					case MODEL_SCORPION_256K:
						if(numPage >= 3 && numPage <= 18) page = PAGE_RAM[numPage - 3];
						break;
				}
				buf += 3;
				sz -= 3;
				if(page) {
					if(!unpackBlock(buf, page, page + 16384, szSrc, szData != 65535, false)) throw(0);
				} else {
					page = page;
				}
				buf += szData;
				sz -= szData;
			}
			writePort(0xfd, 0x7f, head2->hardState);
			memcpy(_AY, head2->sndRegs, 16);
			writePort(0xfd, 0xff, head2->sndChipRegNumber);
			if(head3 && length == 87) writePort(0xfd, 0x1f, head3->port1FFFD);
		} else {
			if(!unpackBlock(buf, TMP_BUF, &TMP_BUF[49152], sz, head1->STATE1 & 32, true)) throw(0);
			// раскидать по страницам
			memcpy(PAGE_RAM[5], TMP_BUF, 16384);
			memcpy(PAGE_RAM[2], &TMP_BUF[16384], 16384);
			memcpy(PAGE_RAM[0], &TMP_BUF[32768], 16384);
		}
		result = true;
	} catch(...) { }

	delete[] ptr;

	return result;
}

static bool packPage(zxFile* f, ssh_b* src, ssh_b page) {
	ssh_d size;

	packBlock(src, src + 16384, &TMP_BUF[3], false, size);
	*(ssh_w*)TMP_BUF = (ssh_w)size;
	TMP_BUF[2] = page;
	size += 3;
	return f->write(TMP_BUF, size);
}

bool zxBus::saveZ80(ssh_cws path) {
	static ssh_b models[] = { 0, 3, 9, 10 };
	static HEAD3_Z80 head;

	auto head2 = &head.head2;
	auto head1 = &head2->head1;
	memset(&head, 0, sizeof(HEAD3_Z80));

	// основные
	head1->BC = *_BC; head1->DE = *_DE; head1->HL = *_HL; head1->AF = *_AF;
	head1->SP = *_SP; head1->IX = *_IX; head1->IY = *_IY; head1->PC = 0;
	head1->I = *_I; head1->R = (*_R) & 127; head1->STATE2 = *_IM; head1->IFF1 = *_IFF1; head1->IFF2 = *_IFF2;
	head1->STATE1 = (((*_R) & 128) >> 7) | (((*_FE) & 7) << 1);
	memcpy(&head1->BC_, &cpuZX[RC_], 8);
	// для режима 128К
	head.port1FFFD = *_1FFD;
	head2->length = 55;
	head2->hardMode = models[*_MODEL];
	head2->hardState = *_7FFD;
	head2->PC = *_PC;
	head2->sndChipRegNumber = *_AY_REG;
	memcpy(head2->sndRegs, _AY, 16);

	auto pages = theApp->bus.countPages(false);
	zxFile f(path, false);
	if(!f.write(&head, sizeof(HEAD3_Z80))) return false;
	// страницы, в зависимости от режима
	if(*_MODEL == MODEL_48K) {
		if(!packPage(&f, PAGE_RAM[5], 8)) return false;
		if(!packPage(&f, PAGE_RAM[2], 4)) return false;
		if(!packPage(&f, PAGE_RAM[0], 5)) return false;
	} else {
		for(int i = 0; i < pages; i++) {
			if(!packPage(&f, PAGE_RAM[i], i + 3)) return false;
		}
	}
	return true;
}
