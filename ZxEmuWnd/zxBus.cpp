
#include "stdafx.h"
#include "zxBus.h"
#include "zxCPU.h"
#include "zxGPU.h"
#include "zxSND.h"
#include "zxDebugger.h"
#include "zxKeyboard.h"

// содержмое памяти
ssh_b memZX[50 + 65536];

// адрес прерывания
ssh_b* _I(&memZX[RI]);

// ренегерация
ssh_b* _R(&memZX[RR]);

// режим прерываний
ssh_b* _IM(&memZX[IM]);

// триггеры
ssh_b* _IFF1(&memZX[IFF1]);
ssh_b* _IFF2(&memZX[IFF2]);

// содержимое порта FE/FD
ssh_b* _PORT_FE(&memZX[FE]);
ssh_b* _LOCK_FD(&memZX[LOCK_FD]);
ssh_b* _KEYS(&memZX[KEYS0]);
ssh_b* _KEMPSTON(&memZX[KEMPSTON]);

// RAM
ssh_b* _RAM(&memZX[RAM]);
// VID
ssh_b* _VID(&memZX[VID]);
// ROM
ssh_b* _ROM(&memZX[ROM]);
// скан строка
ssh_b* _SCAN(&memZX[SCAN]);

// текущая модель памяти
ssh_b* _MODEL(&memZX[MODEL]);

// регистры общего назначения
ssh_b* _A(&memZX[RA]);
ssh_b* _B(&memZX[RB]);

ssh_w* _BC((ssh_w*)&memZX[RC]);
ssh_w* _DE((ssh_w*)&memZX[RE]);
ssh_w* _HL((ssh_w*)&memZX[RL]);
ssh_w* _AF((ssh_w*)&memZX[RF]);
ssh_w* _SP((ssh_w*)&memZX[RSPL]);
ssh_w* _IX((ssh_w*)&memZX[RXL]);
ssh_w* _IY((ssh_w*)&memZX[RYL]);
ssh_w* _PC((ssh_w*)&memZX[RPCL]);
ssh_w* _PC_EXIT_CALL((ssh_w*)&memZX[PC_EXIT_CALL1]);
// статус
ssh_w* _TSTATE((ssh_w*)&memZX[TSTATE1]);

zxBus::~zxBus() {
	SAFE_DELETE(memROM);
	SAFE_DELETE(memRAM);
	bankROM = 0;
	bankRAM = 0;
}

void zxBus::updateData() {
	// задержка ЦПУ и ГПУ
	int nTurbo = (theApp->getOpt(OPT_TURBO)->dval ? 2 : 1);
	delayCPU = (theApp->getOpt(OPT_DELAY_CPU)->dval / nTurbo);
	delayGPU = (theApp->getOpt(OPT_DELAY_GPU)->dval / nTurbo);
	// наличие звука
	isSound = theApp->getOpt(OPT_DELAY_CPU)->dval;
	// громкость звука
	// задержка звука
	periodSND = theApp->getOpt(OPT_PERIOD_SND)->dval;
	// задержка мелькания
	periodBLINK = theApp->getOpt(OPT_PERIOD_BLINK)->dval;
	// задержка бордера
	periodBORDER = theApp->getOpt(OPT_PERIOD_BORDER)->dval;
	// обновить ГПУ
	GPU.updateData();
}

ssh_b* zxBus::getMemBank(ssh_b page, bool rom) {
	if(rom) return (page < bankROM ? (memROM + page * 16384) : nullptr);
	return (page < bankRAM ? (memRAM + page * 16384) : nullptr);
}

void zxBus::swapPage(ssh_b page, TypeSwap type) {
	ssh_b* newMem;
	switch(type) {
		case SWAP_ROM:
			if(page != (*_ROM)) {
				if(newMem = getMemBank(page, true)) {
					if((*_TSTATE) & ZX_WRITE_ROM) {
						auto oldMem = getMemBank(*_ROM, true);
						if(oldMem) memcpy(oldMem, &memZX, 16384);
					}
					memcpy(&memZX, newMem, 16384);
					*_ROM = page;
				}
			}
			break;
		case SWAP_RAM:
			if(page != *_RAM) {
				if(newMem = getMemBank(page, false)) {
					auto oldMem = getMemBank(*_RAM, false);
					if(oldMem) {
						memcpy(oldMem, &memZX[0xc000], 16384);
						//if(*_RAM == 2) memcpy(oldMem, &memZX[0x8000], 16384);
					}
					memcpy(&memZX[0xc000], newMem, 16384);
					//if(page == 2) memcpy(&memZX[0x8000], newMem, 16384);
					*_RAM = page;
				}
			}
			break;
		case SWAP_VRAM:
			if(page != *_VID) {
				if(newMem = getMemBank(page, false)) {
					auto oldMem = getMemBank(*_VID, false);
					if(oldMem) memcpy(oldMem, &memZX[0x4000], 16384);
					memcpy(&memZX[0x4000], newMem, 16384);
					*_VID = page;
				}
				break;
			}
	}
}

bool zxBus::changeModel(int newModel, int oldModel) {
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
			SAFE_DELETE(memROM);
			SAFE_DELETE(memRAM);
			memROM = pROM;
			switch(newModel) {
				case MODEL_48K:
					bankRAM = 0; bankROM = 1;
					break;
				case MODEL_128K:
					bankRAM = 8; bankROM = 2;
					break;
			}
			if(bankRAM) {
				memRAM = new ssh_b[bankRAM * 16384];
				memset(memRAM, 0, bankRAM * 16384);
			}
			signalRESET();
		}
	}
	return true;
}

bool zxBus::loadState(int hf) {
	// грузим основную память
	if(_read(hf, memZX, sizeof(memZX)) != sizeof(memZX)) return false;
	// грузим банки ОЗУ
	if(*_MODEL != MODEL_48K) {
		int banks = countMemBank(false);
		for(int i = 0; i < banks; i++) {
			if(_read(hf, getMemBank(i, false), 16384) != 16384) return false;
		}
	}
	return true;
}

bool zxBus::saveState(int hf) {
	// сохраняем регистры
	if(_write(hf, &memZX[65536], 50) != 50) return false;
	// сохраняем основную память
	if(_write(hf, memZX, sizeof(memZX)) != sizeof(memZX)) return false;
	if(*_MODEL != MODEL_48K) {
		// количество банков и их содержимое
		int banks = countMemBank(false);
		for(int i = 0; i < banks; i++) {
			if(_write(hf, getMemBank(i, false), 16384) != 16384) return false;
		}
	}
	return true;
}

void zxBus::signalRESET() {
	memset(memZX, 0, 65536);
	memset(memRAM, 0, bankRAM * 16384);
	memset(&memZX[RC_], 0, 8);
	memset(_KEYS, 255, 8);
	*_IFF1 = *_IFF2 = *_IM = *_LOCK_FD = *_PORT_FE = 0;
	*_AF = *_BC = *_DE = *_HL = *_IX = *_IY = *_PC = 0;
	*_R = *_I = 0;
	*_SP = 65534;
	*_KEMPSTON = 0;
	*_PC_EXIT_CALL = 0;
	*_RAM = *_VID = *_ROM = -1;
	theApp->opts.nameLoadProg = L"BASIC";
	swapPage(0, SWAP_ROM);
	modifyTSTATE(0, ~(ZX_EXEC | ZX_DEBUG | ZX_BUFFER_GPU));
	theApp->updateCaption();
}

void zxBus::signalINT() {
	if(*_IFF1 && ((*_TSTATE) & ZX_TRAP)) {
		modifyTSTATE(0, ZX_TRAP);
		*_IFF1 = *_IFF2 = 0;
		switch(*_IM) {
			case 0: /*CPU->execCALL(0x66); break;*/
			case 1: CPU.execCALL(0x38); break;
			case 2: CPU.execCALL(CPU.read_mem16((*_I) * 256 + 254)); break;
		}
		CPU.incrementR();
	}
}

void zxBus::signalNMI() {
	_IFF1 = 0;
	CPU.execCALL(0x66);
}

void zxBus::step(bool run_debugger) {
	if((*_TSTATE) & ZX_RESET) signalRESET();
	if(((*_TSTATE) & ZX_EXEC) || run_debugger) {
		modifyTSTATE(ZX_INT, 0);
		if(((*_TSTATE) & ZX_DEBUG) && !run_debugger) {
			if(theApp->debug->checkBPS(*_PC, false)) return;
		}
		// выполнение операции
		CPU.execOps(0, 0);
		// проверка на прерывания
		if((*_TSTATE) & ZX_NMI) signalNMI();
		else if((*_TSTATE) & ZX_INT) signalINT();
	}
}

ssh_d zxBus::execute() {
	LARGE_INTEGER sample;
	QueryPerformanceFrequency(&sample);

	DWORD ms = sample.LowPart / 1000;

	QueryPerformanceCounter(&sample);

	ssh_u sndTm = 0;
	ssh_u startCpu = sample.LowPart;
	ssh_u startBrd = startCpu;
	ssh_u startSnd = startCpu;
	ssh_u startGpu = startCpu;

	while(!((*_TSTATE) & ZX_TERMINATE)) {
		QueryPerformanceCounter(&sample);
		ssh_u current = sample.LowPart;
		if((current - startGpu) > (ms * delayGPU)) {
			theApp->keyboard->processJoystick();
			GPU.showScreen();
			(*_TSTATE) |= ZX_TRAP;
			startGpu = current;
		}
		if(((*_TSTATE) & ZX_EXEC)) {
			if((current - startCpu) > delayCPU) {
				step(false); startCpu = current;
			}
			if((current - startSnd) > (delayCPU * periodSND)) {
				if(isSound) SND.execute(sndTm++);
				startSnd = current;
			}
			if((current - startBrd) > (delayCPU * periodBORDER)) {
				GPU.execute();
				startBrd = current;
			}
		}
	}

	return 0;
}
