#pragma once

struct ZX_DEBUGGER {
	ssh_cws text;
	ssh_d idMain;
	ssh_d idText;
	ssh_b* regb;
	ssh_w* regw;
	ssh_w msk;
	ssh_b shift;
	ssh_w val;
	HWND hWndMain;
	HWND hWndText;
	bool change;
};

#include "zxListBox.h"

class zxAssembler;
class zxDisAsm;
class zxBus;

class zxDebugger : public zxDialog {
	friend class zxDlgListBps;
public:
	enum Updates {
		U_REGS	= 1,
		U_PC	= 2,
		U_SP	= 4,
		U_STORY	= 8,
		U_SEL	= 16,
		U_TOP	= 32
	};
	zxDebugger() : zxDialog(), hWndDA(nullptr), hWndSP(nullptr), da(nullptr), hFont(nullptr), countVisibleItems(0),
					_pc(-1), _sp(-1), _lastPC(-1), _dt(0), curIndexBP(-1), curStoryPC(-1), limitStoryPC(0) { 
		hAccel = LoadAccelerators(hInst, MAKEINTRESOURCE(IDA_ACCEL_DEBUGGER));
	}
	virtual ~zxDebugger();
	bool checkBPS(ssh_w address, bool mem);
	void show(bool visible);
	void updateRegisters(int newPC, int flags);
	bool check(ssh_w address, ZX_BREAK_POINT& breakptr, int flags);
	void saveBPS();
	void removeBP(int index);
	void setProgrammPause(bool pause, bool activate);
protected:
	ssh_msg void onPrevBreakpoint();
	ssh_msg void onNextBreakpoint();
	ssh_msg void onQuckBreakpoint();
	ssh_msg void onListBreakpoint();
	ssh_msg void onStepInto();
	ssh_msg void onStepOver();
	ssh_msg void onPause();
	ssh_msg void onRun();
	ssh_msg void onPcUndo();
	ssh_msg void onPcRedo();
	ssh_msg void onHexDec();
	ssh_msg void onOverProc();
	ssh_msg void onSetPC();
	ssh_msg void onSetSP();
	ssh_msg void onSetData();
	ssh_msg void onSetCmd();
	ssh_msg void onDblkClkListDA();
	ssh_msg void onDblkClkListSP();
	ssh_msg void onScrollListDA();
	ssh_msg void onSelChangeListDA();
	ssh_msg void onChangeEditRegs();

	ssh_msg void onClose();
	ssh_msg void onDrawItem(int id, LPDRAWITEMSTRUCT dis);

	virtual bool preCreate() override;
	virtual void postCreate() override;

	void updateUndoRedo(bool set);
	void updatePrevNextBP();
	void quickBP(int adr);
	void updateStack(int sp);
	void updateHexDec(bool change);
	int comparePredefinedNames(ssh_cws buf);
	int _pc, _sp, _lastPC, _dt;
	int countVisibleItems;
	zxListBox zxDA, zxSP, zxDT;
	zxDisAsm* da;
	zxAssembler* assm;
	HFONT hFont;
	HBRUSH hbrSel, hbrUnsel;
	ZX_BREAK_POINT bps[COUNT_BP];
	ssh_w storyPC[COUNT_STORY_PC];
	HWND hWndDA, hWndSP, hWndDT;
	int curStoryPC, limitStoryPC;
	int curIndexBP;
	DECL_MSG_MAP()
};