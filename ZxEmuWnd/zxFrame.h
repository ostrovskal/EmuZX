#pragma once

#include "zxSettings.h"
#include "zxBus.h"

class zxDebugger;
class zxGamepad;
class zxKeyboard;

class zxFrame : public zxWnd {
	friend class zxBus;
public:
	zxFrame();
	virtual ~zxFrame();
	
	// �������� ���� ��������� ���������
	int run();
	
	// ��������� �������� ����� �� �� ��
	auto getOpt(int idx) const { return opts.get(idx); }

	// ��������/�������� ������ ����/�������
	void updateData(ssh_d change);
	
	// �����/������������ �����
	void pauseCPU(bool isPause);

	// ���������� ������ ��������� ����
	void updateCaption();

	// ���������
	zxSettings	opts;

	// ����
	zxBus bus;

	// ��������
	zxDebugger* debug;

	// ����������
	zxKeyboard*	keyboard;

	// ��������
	zxGamepad*	gamepad;

	// ������� �����������
	RECT wndRect;
protected:
	ssh_msg void onKeyDown(UINT nVirtKey, UINT nRepeat, UINT nFlags);
	ssh_msg void onKeyUp(UINT nVirtKey, UINT nRepeat, UINT nFlags);
	ssh_msg void onDestroy();
	ssh_msg void onSettings();
	ssh_msg void onOpen();
	ssh_msg void onSave();
	ssh_msg void onRestore();
	ssh_msg void onReset();
	ssh_msg void onClose();
	ssh_msg void onProcessMRU();
	ssh_msg void onSize(UINT type, int nWidth, int nHeight);
	ssh_msg void onNotify(LPNMHDR nmNMHDR, LRESULT* pResult);
	ssh_msg int onCalcSize(bool isParams, LPARAM lParam);
	ssh_msg BOOL onEraseBkgnd(HDC hdc);
	ssh_msg void onUpdate();

	bool changeState(int id_opt, int id, bool change);
	bool checkedSubMenu(HMENU hMenu, int id_opt, int val, int* ids);

	void modifyMRU(zxString path);
	void processKeys();
	void makeWndSize(SIZE* sz);
	bool saveState(ssh_cws path);
	bool loadState(ssh_cws path);

	HMENU hMenuMRU, hMenuPP, hMenuModel;
	HANDLE hCpuThread;
	MENUITEMINFO mii;

	DECL_MSG_MAP()
};
