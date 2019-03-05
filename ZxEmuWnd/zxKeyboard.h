#pragma once

class zxKeyboard : public zxDialog {
public:
	enum KeyMode {
		KM_K, KM_L, KM_C, KM_E, KM_G, KM_SH_E, KM_SH_KL, KM_CH,
	};
	enum Key {
		KEY_1 = 1, KEY_2, KEY_3, KEY_4, KEY_5, KEY_6,
		KEY_7, KEY_8, KEY_9, KEY_0, KEY_Q, KEY_W, KEY_E,
		KEY_R, KEY_T, KEY_Y, KEY_U, KEY_I, KEY_O, KEY_P,
		KEY_A, KEY_S, KEY_D, KEY_F, KEY_G, KEY_H, KEY_J,
		KEY_K, KEY_L, KEY_ENTER, KEY_CAPS, KEY_Z, KEY_X,
		KEY_C, KEY_V, KEY_B, KEY_N, KEY_M, KEY_SYMBOL,
		KEY_SPACE, KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT,
		KEY_KEM_UP, KEY_KEM_DOWN, KEY_KEM_LEFT, KEY_KEM_RIGHT, KEY_KEM_FIRE
	};
	struct ZX_KEY {
		int butID;
		ssh_b port, bit;
		ssh_b portEx, bitEx;
		ssh_b vk_code;
		ssh_cws name_k, name_l, name_c, name_e;
		ssh_cws name_sh_e, name_sh_k;
		ssh_cws name_g;
		HWND hWndKey;
	};
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
	ssh_msg void onClose() { theApp->updateData(zxFrame::ST_KEYBOARD); }
	virtual int onInitDialog(HWND hWnd) override;
	void highlightKey(ssh_cws name, ZX_KEY* k, int isVirt);
	HFONT hFont;
	HBRUSH hbrSel, hbrUnsel;
	DECL_MSG_MAP()
};