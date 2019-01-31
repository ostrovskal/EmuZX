#pragma once

enum CPU_REG {
	RC, // 0
	RB, // 1
	RE, // 2
	RD, // 3
	RL, // 4
	RH, // 5
	RA, // 6
	RF, // 7 (HL)
	RC_, // 8
	RB_, // 9
	RE_, // 10
	RD_, // 11
	RL_, // 12
	RH_, // 13
	RA_, // 14
	RF_, // 15
	RIXL, // 16
	RIXH, // 17
	RIYL, // 18
	RIYH, // 19
	RSPL, // 20
	RSPH, // 21
	RI,
	RR,
	RIFF1,
	RIFF2,
	RIM,
	RTRAP,
	RFE,
	RPC_EXIT_CALL1,
	RPC_EXIT_CALL2,
	COUNT_REGS
};

enum CPU_FLAGS {
	FC = 1,
	FN = 2,// SUB,DEC,CMP
	FPV = 4,//P=1->четность бит,V=1->переполнение
	F3 = 8,// не используетс€
	FH = 16,// перенос между 3 и 4 битами
	F5 = 32,// не используетс€
	FZ = 64,
	FS = 128
};

// префиксы
#define PREFIX_NO		0//0
#define PREFIX_CB		1//203
#define PREFIX_ED		2//237
#define PREFIX_IX		1//221
#define PREFIX_IY		2//253

#define GET_FZ(val)				(val == 0) << 6
#define GET_FV(val1, val2)		calcFV(val1, val2)
#define GET_FP(val)				calcFP(val) << 2
#define GET_FN(val)				val << 1
#define GET_FC(val1, val2)		(val1 > val2)
#define DA_PUT(c)				path[pos++] = c

extern ssh_w _PC;
extern volatile ssh_w _TSTATE;

extern ssh_b* _I;
extern ssh_b* _R;
extern ssh_b* _IM;
extern ssh_b* _IFF1;
extern ssh_b* _IFF2;
extern ssh_b* _A;
extern ssh_b* _B;
extern ssh_w* _BC;
extern ssh_w* _DE;
extern ssh_w* _HL;
extern ssh_w* _SP;
extern ssh_w* _PC_EXIT_CALL;

extern ssh_b* _PORT_FE;
extern ssh_b* _TRAP;

extern ssh_b* memZX;
extern ssh_b regsZX[COUNT_REGS];
extern ssh_b portsZX[65536];

#include "SettingsZX.h"

class CpuZX;
class GpuZX;
class BorderZX;
class KeyboardZX;
class SoundZX;
class zxDebugger;
class zxKeyboard;

class zxEmulation : public zxWnd {
	friend ssh_d WINAPI ProcCPU(void* params);
public:
	zxEmulation();
	virtual ~zxEmulation();
	
	// основной цикл сообщений приложни€
	int run();
	
	// получение значени€ опции по ее »ƒ
	auto getOpt(int idx) { return opts.get(idx); }

	// изменить статус отладчика
	void changeWndDebugger(bool change);

	// изменить статус клавиатуры
	void changeWndKeyboard(bool change);

	// изменить режим
	void changeExecute(bool change);

	// ѕауза/возонобление треда
	void pauseCPU(bool isPause, int adding);

	// область главного окна
	RECT wndRect;

	// установки
	SettingsZX	opts;

	// отладчик
	zxDebugger* debug;

	// графическа€ подсистема
	GpuZX*		gpu;

	// клавиатура
	zxKeyboard*	keyboard;

	// процессор
	CpuZX*		zilog;
protected:
	ssh_d procCPU();
	virtual bool onCommand(int wmId, int param, LPARAM lParam) override;
	virtual bool onClose() override;
	virtual bool onSize(WPARAM type, int nWidth, int nHeight) override;
	virtual bool onKey(int nVirtKey, LPARAM keyData, bool pressed) override;
	virtual bool onNotify(LPNMHDR nm) override;
	virtual void postCreate() override;

	void modifyMRU(StringZX path);
	bool checkedModelOrPP(HMENU hMenu, int id_opt, int val, int* ids);
	void changeTurbo(bool change);
	void changeSound(bool change);
	bool changeState(int id_opt, int id, bool change);

	BorderZX*	brd;
	KeyboardZX* keyb;
	SoundZX*	snd;

	ssh_d delayCPU;
	ssh_d delayGPU;

	HMENU hMenuMRU;
	HMENU hMenuPP;
	HMENU hMenuModel;
	HANDLE hCpuThread;
	MENUITEMINFO mii;
};

// вычисление четности
inline ssh_b calcFP(ssh_b val) {
	val = ((val >> 1) & 0x55) + (val & 0x55);
	val = ((val >> 2) & 0x33) + (val & 0x33);
	val = ((val >> 4) & 0x0F) + (val & 0x0F);
	return !(val & 1);
}

// вычисление переполнени€
inline ssh_b calcFV(ssh_b val1, ssh_b val2) {
	if((val1 & 128) != (val2 & 128)) return 4;
	return 0;
}

// вычисление полупереноса
inline ssh_b calcFH(ssh_b val, ssh_b offs, ssh_b fn) {
	ssh_b v = val & 15;
	ssh_b o = offs & 15;
	ssh_b ret = fn ? v - o : v + o;
	return (ret > 15) << 4;
}

// обмен регистров
inline void swapReg(ssh_w* r1, ssh_w* r2) {
	ssh_w a = *r1;
	ssh_w b = *r2;
	*r2 = a;
	*r1 = b;
}
