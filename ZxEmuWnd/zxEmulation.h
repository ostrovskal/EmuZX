#pragma once

enum CPU_REG {
	RC, // 0
	RB, // 1
	RE, // 2
	RD, // 3
	RL, // 4
	RH, // 5
	RF, // 6 (HL)
	RA, // 7
	RC_, // 8
	RB_, // 9
	RE_, // 10
	RD_, // 11
	RL_, // 12
	RH_, // 13
	RF_, // 14
	RA_, // 15
	RXL, // 16
	RXH, // 17
	RYL, // 18
	RYH, // 19
	RSPL, // 20
	RSPH, // 21
	RPCL, // 22
	RPCH, // 23
	RI,
	RR,
	IFF1,
	IFF2,
	IM,
	TRAP,
	FE,
	FD,
	RAM,
	ROM,
	VID,
	SCAN,
	PC_EXIT_CALL1,
	PC_EXIT_CALL2,
	COUNT_REGS
};

enum CPU_FLAGS {
	FC = 1,
	FN = 2,// SUB,DEC,CMP
	FPV = 4,//P=1->четность бит,V=1->переполнение
	//F3 = 8,// не используется
	FH = 16,// перенос между 3 и 4 битами
	//F5 = 32,// не используется
	FZ = 64,
	FS = 128
};

// префиксы
#define PREFIX_NO		0//0
#define PREFIX_CB		1//203
#define PREFIX_ED		2//237
#define PREFIX_IX		1//221
#define PREFIX_IY		2//253

#define _FS(val)			(val & 0x80)
#define _FZ(val)			((val == 0) << 6)
#define _FV1(op1, op2, fc, fn)	calcFV1(op1, op2, fc, fn)
#define _FV2(op1, op2, fc, fn)	calcFV2(op1, op2, fc, fn)
#define _FH(op1, op2, fc, fn)	calcFH(op1, op2, fc, fn)
#define _FP(val)			calcFP(val)
#define _FN(val)			(val << 1)
#define _FC(val1, val2)		(val1 > val2)
#define DA_PUT(c)			path[pos++] = c

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
extern ssh_w* _PC;
extern ssh_w* _IX;
extern ssh_w* _IY;
extern ssh_w* _PC_EXIT_CALL;

extern ssh_b* _PORT_FE;
extern ssh_b* _PORT_FD;

extern ssh_b memZX[65536];
extern ssh_b memBanks[16384 * 8];
extern ssh_b regsZX[COUNT_REGS];
extern ssh_b portsZX[65536];

#include "SettingsZX.h"

class CpuZX;
class GpuZX;
class SoundZX;
class zxDebugger;
class zxKeyboard;
class zxGamepad;

class zxEmulation : public zxWnd {
	friend ssh_d WINAPI ProcCPU(void* params);
public:
	zxEmulation();
	virtual ~zxEmulation();
	
	// основной цикл сообщений приложния
	int run();
	
	// получение значения опции по ее ИД
	auto getOpt(int idx) { return opts.get(idx); }

	// изменить статус отладчика
	void changeWndDebugger(bool change);

	// изменить статус клавиатуры
	void changeWndKeyboard(bool change);

	// изменить режим
	void changeExecute(bool change);

	// Пауза/возонобление треда
	void pauseCPU(bool isPause);

	// область главного окна
	RECT wndRect;

	// установки
	SettingsZX	opts;

	// отладчик
	zxDebugger* debug;

	// графическая подсистема
	GpuZX*		gpu;

	// клавиатура
	zxKeyboard*	keyboard;

	// процессор
	CpuZX*		zilog;

	zxGamepad*	gamepad;
protected:
	ssh_msg void onTimer(UINT_PTR nID);
	ssh_msg void onSettings();
	ssh_msg void onAspectRatio();
	ssh_msg void onOpen();
	ssh_msg void onSave();
	ssh_msg void onRestore();
	ssh_msg void onReset();
	ssh_msg void onTurbo();
	ssh_msg void onSound();
	ssh_msg void onDebugger();
	ssh_msg void onKeyboard();
	ssh_msg void onFilter();
	ssh_msg void onModel();
	ssh_msg void onClose();
	ssh_msg void onPause();
	ssh_msg void onProcessMRU();
	ssh_msg void onSize(UINT type, int nWidth, int nHeight);
	ssh_msg void onKeyDown(UINT nVirtKey, UINT nRepeat, UINT nFlags);
	ssh_msg void onKeyUp(UINT nVirtKey, UINT nRepeat, UINT nFlags);
	ssh_msg void onNotify(LPNMHDR nmNMHDR, LRESULT* pResult);
	ssh_msg int onCalcSize(bool isParams, LPARAM lParam);
	ssh_msg BOOL onEraseBkgnd(HDC hdc);

	void processKeys();
	void modifyMRU(StringZX path);
	bool checkedSubMenu(HMENU hMenu, int id_opt, int val, int* ids);
	void changeTurbo(bool change);
	void changeSound(bool change);
	bool changeState(int id_opt, int id, bool change);
	ssh_d procCPU();

	SoundZX* snd;

	ssh_u delayCPU;
	ssh_u delayGPU;

	HMENU hMenuMRU;
	HMENU hMenuPP;
	HMENU hMenuModel;
	HANDLE hCpuThread;
	MENUITEMINFO mii;

	DECL_MSG_MAP()
};

// вычисление четности
inline ssh_b calcFP(ssh_b val) {
	val = ((val >> 1) & 0x55) + (val & 0x55);
	val = ((val >> 2) & 0x33) + (val & 0x33);
	val = ((val >> 4) & 0x0F) + (val & 0x0F);
	return (!(val & 1) << 2);
}

// вычисление переполнения
inline ssh_b calcFV(int o1, int o2, int mx, int mn, ssh_w fn) {
	if(!fn) {
		if((o2 > 0 && (o1 > (mx - o2))) ||
			(o2 < 0 && (o1 < (mn - o2)))) return 4;
		return 0;
	}
	if((o2 > 0 && (o1 < (mn + o2))) ||
		(o2 < 0 && (o1 > (mx + o2)))) return 4;
	return 0;
}

// вычисление переполнения
inline ssh_b calcFV1(char op1, char op2, ssh_b fc, ssh_w fn) {
	return calcFV(op1, op2 + fc, CHAR_MAX, CHAR_MIN, fn);
}

// вычисление переполнения
inline ssh_b calcFV2(short op1, short op2, ssh_b fc, ssh_w fn) {
	return calcFV(op1, op2 + fc, SHRT_MAX, SHRT_MIN, fn);
}

// вычисление полупереноса
inline ssh_b calcFH(ssh_b op1, ssh_b op2, ssh_b fc, ssh_b fn) {
	ssh_b v = op1 & 15;
	ssh_b o = op2 & 15;
	ssh_b ret = fn ? v - (o + fc) : v + (o + fc);
	return (ret > 15) << 4;
}

// обмен регистров
inline void swapReg(ssh_w* r1, ssh_w* r2) {
	ssh_w a = *r1;
	ssh_w b = *r2;
	*r2 = a;
	*r1 = b;
}
