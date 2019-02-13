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
	
	zxBus() : memROM(nullptr), bankROM(0), memRAM(nullptr), bankRAM(0), isSound(false) { 
	}
	
	virtual ~zxBus();

	void step(bool run_debugger);
	void swapPage(ssh_b page, TypeSwap type);
	void changeTurbo(bool accel);
	void changeSound();
	int countMemBank(bool rom) const { return (rom ? bankROM : bankRAM); }
	ssh_b* getMemBank(ssh_b page, bool rom);
	bool loadState(int hf);
	bool saveState(int hf);
	bool changeModel(int newModel, int oldModel);
	bool saveScreen(ssh_cws path) { return GPU.saveScreen(path); }
protected:
	void signalRESET();
	void signalINT();
	void signalNMI();
	ssh_d execute();

	ssh_d delayCPU, delayGPU;

	bool isSound;

	zxCPU CPU;
	zxGPU GPU;
	zxSND SND;

	// банки ПЗУ
	ssh_b* memROM;
	int bankROM;

	// банки памяти
	ssh_b* memRAM;
	int bankRAM;
};
