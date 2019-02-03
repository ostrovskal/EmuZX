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
								_pc(-1), _sp(-1), _lastPC(-1), curIndexBP(-1), curStoryPC(-1), limitStoryPC(0) { }
	virtual ~zxDebugger();
	bool checkBPS(ssh_w address, bool mem);
	void show(bool visible);
	void updateRegisters(int newPC, int flags);
	bool check(ssh_w address, ZX_BREAK_POINT& breakptr, int flags);
	void saveBPS();
	void removeBP(int index);
	void setProgrammPause(bool pause, bool activate);
protected:
	virtual bool onCommand(int wmId, int param, LPARAM lParam) override;
	virtual bool onSize(WPARAM type, int nWidth, int nHeight) override;
	virtual bool onClose() override;
	virtual bool onDrawItem(UINT idCtl, LPDRAWITEMSTRUCT lpdis) override;
	virtual void onInitDialog(HWND hWnd, LPARAM lParam);
	virtual bool preCreate() override;
	void updateUndoRedo(bool set);
	void updatePrevNextBP();
	void quickBP(int adr);
	void updateStack(int sp);
	void updateHexDec(bool change);
	int _pc, _sp, _lastPC;
	int countVisibleItems;
	zxListBox zxDA;
	zxListBox zxSP;
	zxDisAsm* da;
	zxAssembler* assm;
	HFONT hFont;
	HBRUSH hbrSel, hbrUnSel;
	ZX_BREAK_POINT bps[COUNT_BP];
	ssh_w storyPC[COUNT_STORY_PC];
	HWND hWndDA, hWndSP;
	int curStoryPC, limitStoryPC;
	int curIndexBP;
	DECL_MSG_MAP()
};