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
	
	zxBus() :	pageROM(nullptr), pageRAM(nullptr), isSound(false), delayCPU(10), delayGPU(20), periodSND(1), periodBORDER(16), periodBLINK(15) { 
	}
	
	virtual ~zxBus();

	void step(bool run_debugger);
	void updateData();
	void execute();
	void swapPage(ssh_b page, TypeSwap type);
	bool loadState(int hf);
	bool saveState(int hf);
	bool changeModel(int newModel, int oldModel);
	bool saveScreen(ssh_cws path) { return GPU.saveScreen(path); }
	int countPages(bool rom) const;
	ssh_b* getPage(ssh_b page, bool rom);
protected:
	void signalRESET();
	void signalINT();
	void signalNMI();

	ssh_d delayCPU, delayGPU;
	ssh_d periodSND, periodBLINK, periodBORDER;

	bool isSound;

	zxCPU CPU;
	zxGPU GPU;
	zxSND SND;

	// страницы ПЗУ
	ssh_b* pageROM;
	ssh_b* pageRAM;
};
