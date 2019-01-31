
#pragma once

#include "zxWnd.h"

class zxDlgAddBp : public zxDialog {
public:
	zxDlgAddBp() : zxDialog(), _bp(nullptr), isAddr2(false) {}
	virtual ~zxDlgAddBp() {}
	void edit(ZX_BREAK_POINT* bp) { _bp = bp; }
	ZX_BREAK_POINT result;
protected:
	virtual void onInitDialog(HWND hWnd, LPARAM lParam) override;
	virtual bool onCommand(int wmId, int param, LPARAM lParam) override;
	int getValue(HWND hWnd, int def);
	void updateAccess();
	ZX_BREAK_POINT* _bp;
	HWND hWndAddr1, hWndAddr2;
	HWND hWndValue, hWndOK;
	HWND hWndCond, hWndAccess;
	bool isAddr2;
};
