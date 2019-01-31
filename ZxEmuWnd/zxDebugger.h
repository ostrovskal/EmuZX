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

class zxDisAsm;
class zxDebugger : public zxDialog {
	friend class zxDlgListBps;
public:
	zxDebugger() : zxDialog(), hWndDA(nullptr), hWndSP(nullptr), da(nullptr), hFont(nullptr),
								_pc(-1), _sp(-1), curIndexBP(0), curStoryPC(-1), limitStoryPC(0), owner(false) { }
	virtual ~zxDebugger();
	bool checkBPS(ssh_w address, bool mem);
	void show(bool visible);
	void updateRegisters(int newPC, bool always, bool story);
	bool check(ssh_w address, ZX_BREAK_POINT& breakptr, int flags);
	void saveBPS();
	void removeBP(int index);
	void setProgrammPause(bool pause, bool activate);
protected:
	virtual bool onCommand(int wmId, int param, LPARAM lParam) override;
	virtual bool onClose() override;
	virtual bool onDrawItem(UINT idCtl, LPDRAWITEMSTRUCT lpdis) override;
	virtual void onInitDialog(HWND hWnd, LPARAM lParam);
	virtual INT_PTR onCtlColorStatic(HDC hDC, HWND hWnd) override;
	virtual bool preCreate() override;
	void updateUndoRedo(bool set);
	void updatePrevNextBP();
	void quickBP(int adr);
	void updateStack(int sp);
	void updateHexDec(bool change);
	int _pc, _sp;
	zxListBox zxDA;
	zxListBox zxSP;
	zxDisAsm* da;
	HFONT hFont;
	HBRUSH hbrSel, hbrUnSel;
	ZX_BREAK_POINT bps[10];
	ssh_w storyPC[32];
	HWND hWndDA, hWndSP;
	bool owner;
	int curStoryPC, limitStoryPC;
	int curIndexBP;
	DECL_MSG_MAP()
};