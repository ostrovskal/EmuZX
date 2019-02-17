
#pragma once
#include "zxWnd.h"

struct SHEET_DATA {
	int id;
	int index;
	ssh_cws items;
	int data;
	HWND hWnd;
};

class zxDlgSheet : public zxDialog {
public:
	virtual void default() { onInitDialog(hWnd); }
	virtual void save() { onOK(); }
};

class zxDlgSheetJoystick : public zxDlgSheet {
public:
	zxDlgSheetJoystick() : curJoy(0), curKey(-1) {}
	virtual void default() override;
protected:
	ssh_msg void onSplitButtonDropdown(NMHDR*, LRESULT*);
	ssh_msg void onComboJoyNumber();
	ssh_msg void onComboJoyMapping();
	ssh_msg void onComboJoyAxis();
	ssh_msg void onComboJoyRot();
	ssh_msg void onSplitMenu();
	virtual void onOK() override;
	virtual int onInitDialog(HWND hWnd) override;
	ssh_b status[4][28];
	ssh_b mapping[4];
	int curJoy, curKey;
	DECL_MSG_MAP();
};

class zxDlgSheetSound : public zxDlgSheet {
protected:
	virtual void onOK() override;
	virtual int onInitDialog(HWND hWnd) override;
};

class zxDlgSheetCommon : public zxDlgSheet {
protected:
	virtual void onOK() override;
	virtual int onInitDialog(HWND hWnd) override;
};

class zxDlgSheetGpu : public zxDlgSheet {
public:
	zxDlgSheetGpu() : stkCurrent(nullptr), hFont(0) {}
	virtual ~zxDlgSheetGpu() {
		DeleteObject(hFont);
	}
protected:
	ssh_msg void onButtonColor();
	ssh_msg void onHScroll(UINT code, UINT p, HWND hWndScroll);
	ssh_msg void onDrawItem(int id, LPDRAWITEMSTRUCT dis);
	virtual void onOK() override;
	virtual int onInitDialog(HWND hWnd) override;
	SHEET_DATA* stkCurrent;
	HFONT hFont;
	DECL_MSG_MAP();
};

class zxDlgSettings : public zxDialog {
public:
	zxDlgSettings() : hWndTab(0), pageCurrent(nullptr) {}
protected:
	ssh_msg void onNotifyTabSettings(NMHDR*, LRESULT*);
	ssh_msg void onDefault();
	virtual void onOK() override;
	virtual int onInitDialog(HWND hWnd) override;
		
	zxDlgSheetCommon pageCommon;
	zxDlgSheetSound pageSound;
	zxDlgSheetJoystick pageJoystick;
	zxDlgSheetGpu pageGpu;
	zxDlgSheet* pageCurrent;
	HWND hWndTab;
	DECL_MSG_MAP();
};

