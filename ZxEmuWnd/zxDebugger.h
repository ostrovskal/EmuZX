#pragma once

struct ZX_DEBUGGER {
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

struct ZX_BREAK_POINT {
	int address1;
	int address2;
	int value;
	int flags;
};

class zxDisAsm;
class zxDebugger {
	friend INT_PTR CALLBACK DlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	friend LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
public:
	zxDebugger();
	virtual ~zxDebugger();
	bool checkBPS(ssh_w address, bool mem);
	void show(bool visible);
	void updateRegisters(int newPC, bool always);
protected:
	INT_PTR procWnd(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	INT_PTR procDlg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	void makeToolbar();
	void init(HWND hWnd);
	void drawItem(DRAWITEMSTRUCT* dis, int id);
	void updateStack(int sp);
	void updateHexDec(bool change);
	int topIndexDA, topIndexSP;
	RECT rcWnd;
	RECT rcSP;
	ssh_w firstPC;
	ssh_w lastPC;
	zxDisAsm* da;
	HFONT hFont;
	HBITMAP hBmp;
	HWND hWndToolbar;
	HBRUSH hbrSel, hbrUnSel;
	ZX_BREAK_POINT* bps[10];
};