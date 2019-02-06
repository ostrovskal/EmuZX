
#pragma once

class zxDlgAddBp : public zxDialog {
public:
	zxDlgAddBp() : zxDialog(), _bp(nullptr), isAddr2(false), owner(false) {}
	void edit(ZX_BREAK_POINT* bp) { _bp = bp; }
	ZX_BREAK_POINT result;
protected:
	ssh_msg void onNotifyAddr1(LPNMHDR nm, LRESULT* pResult);
	ssh_msg void onNotifyAddr2(LPNMHDR nm, LRESULT* pResult);
	ssh_msg void onNotifyAccess(LPNMHDR nm, LRESULT* pResult);

	virtual void postCreate() override;
	virtual bool onOK() override;

	int getValue(HWND hWnd, int def);
	void updateAccess();
	ZX_BREAK_POINT* _bp;
	bool isAddr2, owner;

	DECL_MSG_MAP()
};
