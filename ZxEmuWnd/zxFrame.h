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
	
	// основной цикл сообщений приложния
	int run();
	
	// получение значения опции по ее ИД
	auto getOpt(int idx) const { return opts.get(idx); }

	// изменить/обновить статус меню/тулбара
	void updateData(ssh_d change);
	
	// обновление текста заголовка окна
	void updateCaption();

	// установки
	zxSettings	opts;

	// шина
	zxBus bus;

	// отладчик
	zxDebugger* debug;

	// клавиатура
	zxKeyboard*	keyboard;

	// джойстик
	zxGamepad*	gamepad;

	// область отображения
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
	ssh_msg void onSizing(UINT corner, LPRECT rect);
	ssh_msg void onProcessMRU();
	ssh_msg void onSize(UINT type, int nWidth, int nHeight);
	ssh_msg void onNotify(LPNMHDR nmNMHDR, LRESULT* pResult);
	ssh_msg BOOL onEraseBkgnd(HDC hdc);
	ssh_msg void onUpdate();

	bool changeState(int id_opt, int id, bool change);
	bool checkedSubMenu(HMENU hMenu, int id_opt, ssh_d val, int* ids);

	void modifyMRU(zxString path);
	void processKeys();
	long makeWndSize();

	HMENU hMenuMRU, hMenuPP, hMenuModel;
	UINT_PTR hTimer;
	MENUITEMINFO mii;

	DECL_MSG_MAP()
};
