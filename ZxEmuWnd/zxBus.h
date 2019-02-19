#pragma once

#include "zxCPU.h"
#include "zxGPU.h"
#include "zxSND.h"

class zxBus {
	friend ssh_d WINAPI ProcBus(void* params);
public:
	enum TypeSwap {
		SWAP_ROM, SWAP_RAM, SWAP_VRAM
	};
	
	zxBus() :	ROMs(nullptr), RAMs(nullptr), divFreq(35),
				isSound(false), delayCPU(10), delayGPU(20), periodSND(1), periodBORDER(16), periodBLINK(15), isInterleaved(false),
				lastFE16(0), lastFE8(0), tapePos(0), tapeLen(0), tapePause(1), lastTapeOut(0), tCounter(0), z80NTtodo(0), nRetPort(255) {
	}
	
	virtual ~zxBus();

	int step(bool run_debugger, bool allow_int);
	void updateData();
	void execute();
	void z80NT(int todo, int interrupt);
	bool loadState(int hf);
	bool saveState(int hf);
	bool changeModel(int newModel, int oldModel, int divFreq = 35);
	bool saveScreen(ssh_cws path) { return GPU.saveScreen(path); }
	inline int countPages(bool rom) const {
		static int banks[] = {8, 1, 8, 2, 6, 8, 16, 4};
		return banks[*_MODEL * 2 + rom];
	}
	inline ssh_b* getPage(ssh_b page, bool rom, bool isScorpionPage) const {
		if(rom) return &ROMs[page * 16384];
		return PAGE_RAM[page + (isScorpionPage << 3)];
	}

	void swapPage(ssh_b page, TypeSwap type, bool isScorpionPage);
	ssh_b readPort(ssh_b A0A7, ssh_b A8A15);
	void writePort(ssh_b A0A7, ssh_b A8A15, ssh_b val);
	void screenOutNT(ssh_d* vs, ssh_d* colors, bool blink);

	// счетчик тактов процессора
	ssh_d tCounter;
protected:
	void signalRESET();
	int signalINT();
	int signalNMI();

	ssh_d delayCPU, delayGPU;
	ssh_d periodSND, periodBLINK, periodBORDER;

	bool isSound;

	zxCPU CPU;
	zxGPU GPU;
	zxSND SND;

	// страницы ПЗУ и ОЗУ
	ssh_b* ROMs;
	ssh_b* RAMs;
	
	// значение возврата из порта
	ssh_b nRetPort;

	// параметры развертки
	int NT_T_UP, NT_T_LP, NT_T_RP, NT_T_DP, NT_T_INT_LEN;

	// делитель частоты
	int divFreq;

	// скорость эмулятора в процентах от основной(100)
	int emulatorSpeed;

	// задержка между выводом строк экрана
	int z80NTtodo;
	
	// черестрочный вывод экрана
	bool isInterleaved;

	// последнее значение порта магнитофона/динамика
	int lastFE16, lastFE8;

	// переменные для загрузки/записи с ленты
	int tapePos, tapeLen, tapePause, lastTapeOut;
};
