#pragma once

struct ZX_KEY {
	int butID;
	ssh_cws name_k;
	ssh_cws name_l;
	ssh_cws name_c;
	ssh_cws name_e;
	ssh_cws name_sh_e;
	ssh_cws name_sh_k;
	ssh_cws name_g;
	ssh_b port;
	ssh_b bit;
	ssh_b vk_code;
	HWND hWndKey;
};

struct ZX_KEY_EX {
	ssh_b vk_code;
	ssh_b vk_codeKey;
	ssh_b vk_codeKeyEx;
};

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
	ssh_msg void onClose() { theApp.changeWndKeyboard(true); }
	virtual bool preCreate() override;
	virtual void postCreate() override;
	void highlightKey(ssh_cws name, ZX_KEY* k, int isVirt);
	HFONT hFont;
	HBRUSH hbrSel, hbrUnsel;
	DECL_MSG_MAP()
};