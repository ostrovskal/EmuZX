#pragma once

#include "zxCPU.h"
#include "zxGPU.h"
#include "zxSND.h"
#include "zxDisk.h"
#include "zxTape.h"

class zxBus {
public:
	zxBus();
	virtual ~zxBus();
	void stepDebug();
	void updateData();
	void execute();
	void updateFrame(ssh_d* vs, ssh_d* colors, int sizeBorder, bool blink);
	void updateCPU(int todo, int interrupt);
	void writePort(ssh_b A0A7, ssh_b A8A15, ssh_b val);
	bool loadState(ssh_cws path);
	bool saveState(ssh_cws path);
	bool changeModel(ssh_d newModel, ssh_d oldModel);
	bool loadZ80(ssh_cws path);
	bool loadTAP(ssh_cws path) { return TAPE.loadTAP(path); }
	bool loadTZX(ssh_cws path) { return TAPE.loadTZX(path); }
	bool loadWAV(ssh_cws path) { return TAPE.loadWAV(path); }
	bool selectTRD(ssh_cws path, int slot) { return DISK.selectTRD(path, slot); }
	bool saveZ80(ssh_cws path);
	bool saveTAP(ssh_cws path) { return TAPE.saveTAP(path); }
	bool saveTZX(ssh_cws path) { return TAPE.saveTZX(path); }
	bool saveWAV(ssh_cws path) { return TAPE.saveWAV(path); }
	bool saveTRD(ssh_cws path) { return false; }
	bool saveScreen(ssh_cws path) { return GPU.saveScreen(path); }
	int countPages(bool rom) const {
		static int banks[] = {8, 1, 8, 2, 8, 2, 16, 4};
		return banks[*_MODEL * 2 + rom];
	}
	ssh_b readPort(ssh_b A0A7, ssh_b A8A15);
	void setPages();
protected:
	void signalRESET();
	int signalINT();
	int signalNMI();
	int step(bool allow_int);
	ssh_d updateStates();

	// текущий атрибут (порт #FF)
	int nRetPort;

	// текущий цвет границы
	int border;

	// параметры обновления ЦПУ
	int stateUP, stateLP, stateRP, stateDP;

	// текущее приращение ГПУ
	ssh_d periodGPU;

	// статус звука и джойстика
	bool isSound, isJoystick;

	// страницы ПЗУ и ОЗУ
	ssh_b* ROMs;
	ssh_b* RAMs;

	zxCPU CPU;
	zxGPU GPU;
	zxSND SND;
	zxDisk DISK;
	zxTape TAPE;
};
