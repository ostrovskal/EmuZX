
#pragma once
#include "zxWnd.h"

struct SHEET_DATA {
	int id;
	int index;
	ssh_cws items;
	int data;
	HWND hWnd;
};

class zxDlgSheetJoystick : public zxDialog {
public:
	virtual void onOK() override;
protected:
	ssh_msg void onSplitButtonDropdown(NMHDR*, LRESULT*);
	ssh_msg void onComboJoyNumber();
	ssh_msg void onComboJoyMapping();
	ssh_msg void onComboJoyAxis();
	ssh_msg void onComboJoyRot();
	virtual int onInitDialog(HWND hWnd) override;
	ssh_b map[4][28];
	ssh_b status[4];
	DECL_MSG_MAP();
};

class zxDlgSheetSound : public zxDialog {
public:
	virtual void onOK() override;
protected:
	virtual int onInitDialog(HWND hWnd) override;
};

class zxDlgSheetCommon : public zxDialog {
public:
	virtual void onOK() override;
protected:
	virtual int onInitDialog(HWND hWnd) override;
};

class zxDlgSheetGpu : public zxDialog {
public:
	zxDlgSheetGpu() : stkCurrent(nullptr), hFont(0) {}
	virtual ~zxDlgSheetGpu() {
		DeleteObject(hFont);
	}
	virtual void onOK() override;
protected:
	ssh_msg void onButtonColor();
	ssh_msg void onHScroll(UINT code, UINT p, HWND hWndScroll);
	ssh_msg void onDrawItem(int id, LPDRAWITEMSTRUCT dis);
	virtual int onInitDialog(HWND hWnd) override;
	SHEET_DATA* stkCurrent;
	HFONT hFont;
	DECL_MSG_MAP();
};

class zxDlgSettings : public zxDialog {
public:
	zxDlgSettings() : hWndTab(0), pageCurrent(nullptr) {}
	virtual void onOK() override;
protected:
	ssh_msg void onNotifyTabSettings(NMHDR*, LRESULT*);
	virtual int onInitDialog(HWND hWnd) override;
		
	zxDlgSheetCommon pageCommon;
	zxDlgSheetSound pageSound;
	zxDlgSheetJoystick pageJoystick;
	zxDlgSheetGpu pageGpu;
	zxDialog* pageCurrent;
	HWND hWndTab;
	DECL_MSG_MAP();
};

