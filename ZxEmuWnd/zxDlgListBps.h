
#pragma once

class zxDlgListBps : public zxDialog {
public:
	zxDlgListBps() : zxDialog(), hWndList(nullptr), itemSelected(-1) { }
protected:
	virtual bool onCommand(int wmId, int param, LPARAM lParam) override;
	virtual void onInitDialog(HWND hWnd, LPARAM lParam) override;
	virtual void postCreate() override;
	virtual bool onNotify(LPNMHDR nm) override;
	void setItems();
	void updateItems();
	HWND hWndList;
	int itemSelected;
};