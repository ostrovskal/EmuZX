#pragma once

class zxKeyboard : public zxDialog {
public:
	zxKeyboard() : mode(-1), hFont(nullptr) {
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
	ssh_b vkKeys[256];
protected:
	virtual bool onClose() override { theApp.changeWndKeyboard(true); return false; }
	virtual void onInitDialog(HWND hWnd, LPARAM lParam) override;
	virtual bool preCreate() override;
	HFONT hFont;
	HBRUSH hbrSel, hbrUnsel;
	HPEN hPen;
	int mode;
};