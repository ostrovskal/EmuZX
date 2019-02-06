
#pragma once

#define LBN_VSCROLL		6

class zxListBox : public zxWnd {
public:
	zxListBox() : zxWnd(),	hFont(nullptr), hbrSel(nullptr), hbrUnsel(nullptr), isFont(true), countItems(0), 
							heightItem(20), topIndex(0), curSel(-1), countVisibleItems(0) { }
	virtual ~zxListBox() { if(isFont) DeleteObject(hFont); DeleteObject(hbrSel); DeleteObject(hbrUnsel); }
protected:

	virtual INT_PTR wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) override;

	ssh_msg BOOL onMouseWheel(UINT vkKey, short delta, POINT pt);
	ssh_msg void onLButtonDown(UINT vkKey, POINT pt);
	ssh_msg void onLButtonDblClk(UINT vkKey, POINT pt);
	ssh_msg void onVScroll(UINT code, UINT p, HWND hWndScroll);
	
	
	ssh_msg void onSize(UINT type, int nWidth, int nHeight);
	ssh_msg void onKeyDown(UINT nVirtKey, UINT nRepeat, UINT nFlags);
	ssh_msg void onPaint();
	ssh_msg void onSetFont(HFONT hF, BOOL redraw);


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

	DECL_MSG_MAP()
};
