
#pragma once

class zxWnd {
	friend LRESULT CALLBACK MainWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
public:
	zxWnd();
	virtual ~zxWnd();
	void attach(HWND hWnd);
	void detach();
	static zxWnd* fromHWND(HWND hWnd);
	HWND getHWND() const { return hWnd; }
	virtual bool create(LPCWSTR className, LPCWSTR windowName, DWORD dwStyle, const RECT& rect, zxWnd* pParentWnd, UINT nID, UINT nMenu);
	void showWindow(bool show) const { ShowWindow(hWnd, show); }
	void updateWindow() const { UpdateWindow(hWnd); }
	bool isWindowVisible() const { return IsWindowVisible(hWnd); }
	LRESULT SendMsg(UINT msg, WPARAM wParam, LPARAM lParam) const { return SendMessage(hWnd, msg, wParam, lParam); }
	LONG getStyles() const { return GetWindowLong(hWnd, GWL_STYLE); }
	operator const HWND() { return hWnd; }
protected:
	virtual INT_PTR wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	virtual bool onCommand(int wmId, int param, LPARAM lParam) { return false; }
	virtual bool onClose() { return true; }
	virtual bool onCreate(LPCREATESTRUCT cs) { return true; }
	virtual bool onSize(WPARAM type, int nWidth, int nHeight) { return false; }
	virtual bool onKey(int nVirtKey, LPARAM keyData, bool pressed) { return false; }
	virtual bool onNotify(LPNMHDR nm) { return false; }
	virtual void onIdle() {}
	virtual void onDestroy() {}
	virtual bool onDrawItem(UINT idCtl, LPDRAWITEMSTRUCT lpdis) { return false; }
	virtual bool onScroll(UINT code, UINT pos, HWND hWnd, bool vertical) { return false; }
	virtual bool preCreate() { return true; }
	virtual void postCreate() {  }
	bool makeToolbar(WORD IDB, TBBUTTON* tbb, int cBitmaps, int cButtons, int cxButton, int cyButton);
	ssh_cws registerClass(ssh_cws name, int idMenu, WNDPROC proc);
	zxWnd* parent;
	HWND hWnd;
	HWND hWndToolbar;
	HBITMAP hBmp;
};

class zxDialog : public zxWnd {
public:
	zxDialog() : zxWnd(), nResult(0) {}
	virtual ~zxDialog();
	int create(WORD IDD, zxWnd* wndParent, bool modal);
protected:
	virtual INT_PTR wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) override;
	virtual void onInitDialog(HWND hWnd, LPARAM lParam) {}
	virtual INT_PTR onCtlColorStatic(HDC hDC, HWND hWnd) { return 0; }
	virtual bool onClose() { if(!zxWnd::onClose()) return 0; nResult = IDCANCEL; return false; }
	int nResult;
};
