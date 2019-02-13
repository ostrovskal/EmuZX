
#pragma once

class zxDebugger;
class zxDlgListBps : public zxDialog {
public:
	zxDlgListBps() : zxDialog(), debug(nullptr), hWndList(nullptr), itemSelected(-1) { 
		hAccel = LoadAccelerators(hInst, MAKEINTRESOURCE(IDA_ACCEL_LIST_BPS));
	}
protected:
	ssh_msg void onUpDown();
	ssh_msg void onNewEdit();
	ssh_msg void onRemove();
	ssh_msg void onSave();
	ssh_msg void onClickList(LPNMHDR nm, LRESULT* pResult);
	virtual void postCreate() override;
	virtual void onOK() override;
	void setItems();
	void updateItems();
	HWND hWndList;
	int itemSelected;
	zxDebugger* debug;
	DECL_MSG_MAP()
};