
#pragma once

class zxDlgAddBp : public zxDialog {
public:
	zxDlgAddBp() : zxDialog(), _bp(nullptr), isAddr2(false), owner(false) {}
	void edit(ZX_BREAK_POINT* bp) { _bp = bp; }
	ZX_BREAK_POINT result;
protected:
	ssh_msg void onNotifyAddr1();
	ssh_msg void onNotifyAddr2();
	ssh_msg void onNotifyAccess();

	virtual void postCreate() override;
	virtual void onOK() override;

	int getValue(HWND hWnd, int def);
	void updateAccess();

	ZX_BREAK_POINT* _bp;
	bool isAddr2, owner;

	DECL_MSG_MAP()
};
