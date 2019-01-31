
#pragma once

#define LBN_VSCROLL		6

class zxListBox : public zxWnd {
public:
	zxListBox() : zxWnd(),	hFont(nullptr), hbrSel(nullptr), hbrUnsel(nullptr), isFont(true), countItems(0), 
							heightItem(20), topIndex(0), curSel(-1), countVisibleItems(0) { }
	virtual ~zxListBox() { if(isFont) DeleteObject(hFont); DeleteObject(hbrSel); DeleteObject(hbrUnsel); }
protected:
	virtual INT_PTR wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) override;
	virtual bool onScroll(UINT code, UINT pos, HWND hWnd, int type) override;
	virtual bool onMouseWheel(int vkKey, int x, int y, int delta) override;
	virtual bool onMouseButtonDown(int vkKey, int x, int y, bool left) override;
	virtual bool onMouseButtonDblClk(int vkKey, int x, int y, bool left) override;
	virtual bool onSize(WPARAM type, int nWidth, int nHeight) override;
	virtual bool onPaint() override;
	virtual void onFont(HFONT hF, bool redraw) override;
	virtual bool onKey(int nVirtKey, LPARAM keyData, bool pressed) override;
	virtual void postCreate() override;
	void changeScroll(int& offs, int count, int page);
	void sendNotify(int code);
	int heightItem;
	int countItems;
	int countVisibleItems;
	int topIndex;
	int curSel;
	bool isFont;
	RECT rectWnd;
	HFONT hFont;
	SCROLLINFO si;
	HBRUSH hbrSel, hbrUnsel;

	//DECL_MSG_MAP()
};
