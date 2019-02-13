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
	auto getOpt(int idx) { return opts.get(idx); }

	// �������� ������ ���������
	void changeWndDebugger(bool change);

	// �������� ������ ����������
	void changeWndKeyboard(bool change);

	// �������� �����
	void changeExecute(bool exec, bool change);

	// �����/������������ �����
	void pauseCPU(bool isPause);

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
	ssh_msg void onAspectRatio();
	ssh_msg void onOpen();
	ssh_msg void onSave();
	ssh_msg void onRestore();
	ssh_msg void onReset();
	ssh_msg void onTurbo();
	ssh_msg void onSound();
	ssh_msg void onDebugger();
	ssh_msg void onKeyboard();
	ssh_msg void onFilter();
	ssh_msg void onModel();
	ssh_msg void onClose();
	ssh_msg void onPause();
	ssh_msg void onProcessMRU();
	ssh_msg void onSize(UINT type, int nWidth, int nHeight);
	ssh_msg void onNotify(LPNMHDR nmNMHDR, LRESULT* pResult);
	ssh_msg int onCalcSize(bool isParams, LPARAM lParam);
	ssh_msg BOOL onEraseBkgnd(HDC hdc);

	void modifyMRU(zxString path);
	bool checkedSubMenu(HMENU hMenu, int id_opt, int val, int* ids);
	void changeTurbo(bool change);
	void changeSound(bool change);
	bool changeState(int id_opt, int id, bool change);
	void processKeys();
	void makeWndSize(SIZE* sz);
	bool saveState(ssh_cws path);
	bool loadState(ssh_cws path);

	HMENU hMenuMRU;
	HMENU hMenuPP;
	HMENU hMenuModel;
	HANDLE hCpuThread;
	MENUITEMINFO mii;

	DECL_MSG_MAP()
};
