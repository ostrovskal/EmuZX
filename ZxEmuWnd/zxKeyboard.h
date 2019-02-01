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

#define KM_K		0
#define KM_L		1
#define KM_C		2
#define KM_E		3
#define KM_G		4
#define KM_SH_E		5
#define KM_SH_KL	6
#define KM_CH		7

class zxKeyboard : public zxDialog {
	friend ssh_d WINAPI KeyProc(void* params);
public:
	zxKeyboard() : mode(-1), hKeyThread(nullptr), hFont(nullptr) {
		memset(vkKeys, 0, 256);
	}
	virtual ~zxKeyboard() { 
		CloseHandle(hKeyThread); 
		DeleteObject(hbrSel);
		DeleteObject(hbrUnsel);
		DeleteObject(hFont);
	}
	// обработка нажатий клавиш, режимов клавиатуры и подсветки нажатых
	void processKeys();
	// показать/скрыть окно
	void show(bool visible);
	ssh_b vkKeys[256];
protected:
	virtual bool onClose() override { theApp.changeWndKeyboard(true); return false; }
	virtual void onInitDialog(HWND hWnd, LPARAM lParam) override;
	virtual bool preCreate() override;
	virtual bool onDrawItem(UINT idCtl, LPDRAWITEMSTRUCT lpdis);
	DWORD procKEY();
	HANDLE hKeyThread;
	HFONT hFont;
	HBRUSH hbrSel, hbrUnsel;
	int mode;
};