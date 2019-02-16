#pragma once

#define KM_K		0
#define KM_L		1
#define KM_C		2
#define KM_E		3
#define KM_G		4
#define KM_SH_E		5
#define KM_SH_KL	6
#define KM_CH		7

class zxKeyboard : public zxDialog {
public:
	zxKeyboard() : hFont(nullptr) {
		memset(vkKeys, 0, 256);
	}
	virtual ~zxKeyboard() { 
		DeleteObject(hbrSel);
		DeleteObject(hbrUnsel);
		DeleteObject(hFont);
	}
	// обработка нажатий клавиш, режимов клавиатуры и подсветки нажатых
	void processKeys();
	// показать/скрыть окно
	void show(bool visible);
	// обработать кнопки джойстика
	void processJoystick();
	ssh_b vkKeys[256];
protected:
	ssh_msg void onClose() { theApp->updateData(ST_KEYBOARD); }
	virtual int onInitDialog(HWND hWnd) override;
	void highlightKey(ssh_cws name, ZX_KEY* k, int isVirt);
	HFONT hFont;
	HBRUSH hbrSel, hbrUnsel;
	DECL_MSG_MAP()
};