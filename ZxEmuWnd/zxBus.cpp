
#include "stdafx.h"
#include "zxBus.h"
#include "zxCPU.h"
#include "zxGPU.h"
#include "zxSND.h"
#include "zxDebugger.h"
#include "zxKeyboard.h"

static int modelParams[] = {
	8960, 8, 56, 7168, 69888,// 16192
	8892, 8, 60, 7296, 70908,// 16256
	12544,52,12, 5376, 71680,// 17984
	8960, 48,16, 7168, 69888 // 16192
};

static ssh_b border = 7;

ssh_b* pageROM(nullptr);
ssh_b* pageRAM(nullptr);
ssh_b* pageVRAM(nullptr);
ssh_b* pageATTRIB(nullptr);

ssh_b* PAGE_RAM[64];


// содержмое памяти
ssh_b cpuZX[COUNT_REGS];

bool isStartThread = true;

// адрес прерывания
ssh_b* _I(&cpuZX[RI]);

// ренегерация
ssh_b* _R(&cpuZX[RR]);

// режим прерываний
ssh_b* _IM(&cpuZX[IM]);

// триггеры
ssh_b* _IFF1(&cpuZX[IFF1]);
ssh_b* _IFF2(&cpuZX[IFF2]);

// содержимое порта FE/FD
ssh_b* _PORT_FE(&cpuZX[PORT_FE]);
ssh_b* _PORT_1F(&cpuZX[PORT_1F]);
ssh_b* _PORT_FD(&cpuZX[PORT_FD]);
ssh_b* _KEYS(&cpuZX[KEYS0]);
ssh_b* _KEMPSTON(&cpuZX[KEMPSTON]);

// RAM
ssh_b* _RAM(&cpuZX[RAM]);
// VID
ssh_b* _VID(&cpuZX[VID]);
// ROM
ssh_b* _ROM(&cpuZX[ROM]);
// скан строка
ssh_b* _SCAN(&cpuZX[SCAN]);

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
ssh_w* _PC_EXIT_CALL((ssh_w*)&cpuZX[PC_EXIT_CALL1]);
// статус
ssh_w* _TSTATE((ssh_w*)&cpuZX[TSTATE1]);

zxBus::~zxBus() {
	SAFE_DELETE(pageROM);
	SAFE_DELETE(pageRAM);
}

void zxBus::updateData() {
	// задержка ЦПУ и ГПУ
	int nTurbo = (theApp->getOpt(OPT_TURBO)->dval ? 2 : 1);
	delayCPU = (theApp->getOpt(OPT_DELAY_CPU)->dval / nTurbo);
	delayGPU = (theApp->getOpt(OPT_DELAY_GPU)->dval / nTurbo);
	// черестрочный вывод
	isInterleaved = theApp->getOpt(OPT_INTERLEAVED)->dval;
	// наличие звука
	isSound = theApp->getOpt(OPT_DELAY_CPU)->dval;
	// запись в ROM
	//modifyTSTATE(ZX_WRITE_ROM * theApp->getOpt(OPT_WRITE_ROM)->dval, 0);
	// задержка звука
	periodSND = theApp->getOpt(OPT_PERIOD_SND)->dval;
	// задержка мелькания
	periodBLINK = theApp->getOpt(OPT_PERIOD_BLINK)->dval;
	// задержка бордера
	periodBORDER = theApp->getOpt(OPT_PERIOD_BORDER)->dval;
	// обновить ГПУ
	GPU.updateData();
	// громкость звука
	SND.beeperVolume = theApp->getOpt(OPT_SND_BEEPER_VOL)->dval;
	// обновить звук
	SND.setTicksPerFrame(delayCPU);
}

void zxBus::swapPage(ssh_b page, TypeSwap type, bool isScorpionPage) {
	switch(type) {
		case SWAP_ROM:
			*_ROM = page;
			pageROM = getPage(page, true, false);
			break;
		case SWAP_RAM:
			*_RAM = page;
			pageRAM = getPage(page, false, isScorpionPage);
			break;
		case SWAP_VRAM:
			*_VID = page;
			pageVRAM = getPage(page, false, false);
			pageATTRIB = pageVRAM + 6144;
	}
}

bool zxBus::changeModel(int newModel, int oldModel, int arg) {
	if(newModel != oldModel) {
		// грузим ПЗУ
		int hf;
		zxString pathROM(theApp->opts.mainDir + L"Roms\\" + nameROMs[newModel] + L".rom");
		UINT szROM = 0;
		ssh_b* pROM = nullptr;
		try {
			_wsopen_s(&hf, pathROM, _O_RDONLY | _O_BINARY, _SH_DENYRD, _S_IREAD);
			if(hf == -1) throw(0);
			szROM = _filelength(hf);
			pROM = new ssh_b[szROM];
			if(_read(hf, pROM, szROM) != szROM) throw(0);
		} catch(...) {
			// не удалось загрузить ПЗУ
			SAFE_DELETE(pROM);
		}
		SAFE_CLOSE1(hf);
		if(pROM) {
			// имя загруженной проги по умолчанию
			theApp->opts.nameLoadProg = L"BASIC";
			theApp->getOpt(OPT_MEM_MODEL)->dval = newModel;
			*_MODEL = newModel;
			SAFE_DELETE(ROMs);
			SAFE_DELETE(RAMs);
			ROMs = pROM;
			divFreq = arg;
			// устанавливаем параметры развертки
			auto params = &modelParams[newModel * 5];
			NT_T_UP = params[0] * arg / 35;
			NT_T_LP = params[1] * arg / 35;
			NT_T_RP = params[2] * arg / 35;
			NT_T_DP = params[3] * arg / 35;
			NT_T_INT_LEN = params[4] * arg / 35;
			modifyTSTATE((newModel != MODEL_48K) * ZX_128K, ZX_128K);
			auto pages = countPages(false);
			RAMs = new ssh_b[pages * 16384];
			for(int i = 0; i < pages; i++) PAGE_RAM[i] = (RAMs + i * 16384);
		}
	}
	signalRESET();
	return true;
}

bool zxBus::loadState(int hf) {
	// грузим банки ОЗУ
	int banks = countPages(false);
	for(int i = 0; i < banks; i++) {
		if(_read(hf, PAGE_RAM[i], 16384) != 16384) return false;
	}
	return true;
}

bool zxBus::saveState(int hf) {
	// сохраняем регистры
	if(_write(hf, &cpuZX, sizeof(cpuZX)) != sizeof(cpuZX)) return false;
	// количество банков и их содержимое
	int banks = countPages(false);
	for(int i = 0; i < banks; i++) {
		if(_write(hf, PAGE_RAM[i], 16384) != 16384) return false;
	}
	return true;
}

void zxBus::signalRESET() {
	makeTblParity();
	SND.stop();
	memset(RAMs, 0, countPages(false) * 16384);
	memset(&cpuZX[RC_], 0, 8);
	memset(_KEYS, 255, 8);
	*_IFF1 = *_IFF2 = *_IM = *_PORT_1F = 0;
	*_BC = *_DE = *_HL = *_IX = *_IY = *_PC = *_AF = 0;
	*_R = *_I = 0;
	*_SP = 65534;
	*_PORT_FD = (((*_TSTATE) & ZX_128K) ? 0 : 32);
	*_KEMPSTON = 0;
	*_PC_EXIT_CALL = 0;
	*_PORT_FE = 0b11100111;
	theApp->opts.nameLoadProg = L"BASIC";
	swapPage(0, SWAP_ROM, false);
	swapPage(0, SWAP_RAM, false);
	swapPage(5, SWAP_VRAM, false);
	modifyTSTATE(0, ~(ZX_128K | ZX_EXEC | ZX_DEBUG | ZX_BUFFER_GPU));
	theApp->updateCaption();
}

int zxBus::signalINT() {
	if(*_IFF1 && ((*_TSTATE) & ZX_TRAP)) {
		modifyTSTATE(0, ZX_TRAP | ZX_HALT);
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
	_IFF1 = 0;
	CPU.execCALL(0x66);
	return 11;
}

int zxBus::step(bool run_debugger, bool allow_int) {
	modifyTSTATE(allow_int * ZX_INT, 0);
	if(((*_TSTATE) & ZX_DEBUG) && !run_debugger) {
		if(theApp->debug->checkBPS(*_PC, false)) return 0;
	}
	// выполнение операции
	ticks = 0;
	CPU.execOps(0, 0);
	// проверка на прерывания
	if((*_TSTATE) & ZX_NMI) ticks += signalNMI();
	else if((*_TSTATE) & ZX_INT) ticks += signalINT();
	return ticks;
}

void zxBus::execute() {
	// опрос процессора, отображение экрана, воспроизведение звука, опрос джойстика
	if((*_TSTATE) & ZX_RESET) signalRESET();
	if(*_TSTATE & ZX_EXEC) {
		(*_TSTATE) |= ZX_TRAP;
		theApp->keyboard->processJoystick();
		GPU.execute();
		if(isSound) {
			//SND.execute();
			//SND.play();
		}
	}
	/*
	LARGE_INTEGER sample;
	static DWORD ms;
	static ssh_u startBrd, startSnd, startGpu, startTrap, current;

	if(isStartThread) {
		isStartThread = false;
		startBrd = current;
		startSnd = current;
		startGpu = current;
		startTrap = current;
		current = 0;
		QueryPerformanceFrequency(&sample);
		ms = sample.LowPart / 1000;
	}
	current++;
	if((current - startTrap) > ms) {
		theApp->keyboard->processJoystick();
		startTrap = current;
	}
	if((current - startGpu) > ms) {
		GPU.showScreen();
		startGpu = current;
	}
	if(((*_TSTATE) & ZX_EXEC)) {
		step(false, true);
		if((current - startSnd) > periodSND) {
			startSnd = current;
		}
		if((current - startBrd) > periodBORDER) {
			GPU.execute(false);
			startBrd = current;
		}
	}
	*/
}

ssh_b zxBus::readPort(ssh_b A0A7, ssh_b A8A15) {
	switch(A0A7) {
		case 0xfd:
			switch(A8A15) {
				case 0xff: return SND.curRegsAY[SND.registerLatch];
				case 0xbf: return 0;
				case 0x7f: return 0;
			}
			break;
		case 0x1f: return *_KEMPSTON;
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
	switch(A0A7) {
		// kempston
		case 0x1f: *_KEMPSTON = val; break;
			// covox
		case 0xfb: break;
			// specdrum
		case 0xdf: break;
			// 0, 1, 2 - бордер
			// 3 MIC - при записи/загрузке
			// 4 - SOUND
		case 0xfe: {
			*_PORT_FE &= 224;
			*_PORT_FE |= (val & 31);
			border = val & 7;
			lastTapeOut = (val & 0x8 ? 250 : 5);
			if((tapePos >= tapeLen) || tapePause) {
				int msk = 16;
				if((val & 16) != lastFE16) {
					lastFE16 = val & 16;
					SND.writeNVRegAY(SND_BEEPER, lastFE16);
				}
				if((val & 8) != lastFE8) {
					lastFE8 = val & 8;
					SND.writeNVRegAY(SND_BEEPER, lastFE8);
				}
			}
		}
		break;
		// 1, 2, 4 - страница 0-7
		// 8 - экран 5/7
		// 16 - ПЗУ 0 - 128К 1 - 48К
		// 32 - блокировка
		case 0xfd:
			switch(A8A15) {
				case 0xff: SND.registerLatch = val & 0x0f; break;
				case 0xbf: if((tapePos >= tapeLen) || tapePause) SND.writeRegAY(SND.registerLatch, val); break;
				// SCORPION
				// 0 -> 1 - RAM0(!!), 0 - see bit 1 etc
				// 1 -> 1 - ROM 2, 0 - ROM from 0x7FFD
				// 4 -> 1 - RAM SCORPION, 0 - from 0x7FFD
				case 0x1f:
					*_PORT_1F = val;
					if(val & 2) pageROM = getPage(3, true, false);
					else if(val & 1) pageROM = PAGE_RAM[0];
					else pageROM = getPage(*_ROM, false, false);
					swapPage(*_RAM, SWAP_RAM, *_PORT_1F & 16);
				   break;
				case 0x7f: {
					if(!(*_TSTATE & ZX_128K) || (*_PORT_FD) & 32) break;
					swapPage(val & 8 ? 7 : 5, SWAP_VRAM, false);
					bool isSwapROM = true;
					if(*_MODEL == MODEL_SCORPION_256K) isSwapROM = !(*_PORT_1F & 2);
					if(isSwapROM) swapPage((val & 16) >> 4, SWAP_ROM, false);
					swapPage(val & 7, SWAP_RAM, *_PORT_1F & 16);
					*_PORT_FD = val;
					if(val & 32) *_TSTATE &= ~ZX_128K;
				}
				break;
		}
	}
}

void zxBus::z80NT(int todo, int interrupt) {
	// количество тактов на инструкцию
	todo += z80NTtodo;
	if(interrupt) {
		if(z80NTtodo > 0) {
			step(false, false);
//			tapeF(ticks);
			tCounter += ticks;
			todo -= ticks;
		}
		step(false, true);
//		tapeF(ticks);
		tCounter += ticks;
		todo -= ticks;
	}
	while(true) {
		if(todo < 4) {
			z80NTtodo = todo;
			break;
		}
		step(false, false);
//		tapeF(ticks);
		tCounter += ticks;
		todo -= ticks;
	}
}

#define Sizeof 1

void zxBus::screenOutNT(ssh_d* dest, ssh_d* colors, bool blink) {
	ssh_i line;
	int ink, paper;
	int rb, rc, tmp;

	nRetPort = 255;

	auto b_dest = dest;

	z80NT(NT_T_UP, 1);

	// верхняя часть бордера
	for(int line = 0 ; line < SIZE_BORDER; line++) {
		z80NT(NT_T_LP, 0);
		for(int i = 0 ; i < 80; i++) {
			z80NT(1, 0);
			*dest++ = colors[border]; *dest++ = colors[border];
			*dest++ = colors[border]; *dest++ = colors[border];
		}
		z80NT(NT_T_RP, 0);
	}
	// экран по центру и бордер слева и справа
	for(line = 0; line < HEIGHT_SCREEN; line++) {
		z80NT(NT_T_LP, 0);
		for(int i = 0 ; i < 8; i++) {
			z80NT(1, 0);
			*dest++ = colors[border]; *dest++ = colors[border];
			*dest++ = colors[border]; *dest++ = colors[border];
		}
		
		rb = ((line & 192) * 32 + (line & 7) * 256 + (line & 56) * 4);

		for(int ri = 0; ri < 32; ri++) {
			rc = ri + 32 * (line / 8);
			nRetPort = pageATTRIB[rc];
			ink = (nRetPort & 7) | (nRetPort & 64) >> 3;
			paper = (nRetPort & 120) >> 3;
			if((nRetPort & 128) && blink) { tmp = ink; ink = paper; paper = tmp; }
			z80NT(1, 0);
			*dest++ = (pageVRAM[rb] & 128 ? ink : paper);
			*dest++ = (pageVRAM[rb] & 64 ? ink : paper);
			z80NT(1, 0);
			*dest++ = (pageVRAM[rb] & 32 ? ink : paper);
			*dest++ = (pageVRAM[rb] & 16 ? ink : paper);
			z80NT(1, 0);
			*dest++ = (pageVRAM[rb] & 8 ? ink : paper);
			*dest++ = (pageVRAM[rb] & 4 ? ink : paper);
			z80NT(1, 0);
			*dest++ = (pageVRAM[rb] & 2 ? ink : paper);
			*dest++ = (pageVRAM[rb] & 1 ? ink : paper);
			rb++;
		}
		for(int i = 0; i < 8; i++) {
			z80NT(1, 0);
			*dest++ = colors[border]; *dest++ = colors[border];
			*dest++ = colors[border]; *dest++ = colors[border];
		}
		z80NT(NT_T_RP, 0);
	}
	// нижняя часть бордера
	for(int line = 0 ; line < SIZE_BORDER; line++) {
		z80NT(NT_T_LP, 0);
		for(int i = 0 ; i < 80; i++) {
			z80NT(1, 0);
			*dest++ = colors[border]; *dest++ = colors[border];
			*dest++ = colors[border]; *dest++ = colors[border];
		}
		z80NT(NT_T_RP, 0);
	}
	z80NT(NT_T_DP, 0);
	ssh_u diff = dest - b_dest;
	diff = diff;
}
