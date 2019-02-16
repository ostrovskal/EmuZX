
#pragma once

struct SSH_MSGMAP;
struct SSH_MSGMAP_ENTRY;

class zxWnd {
	friend LRESULT CALLBACK MainWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	friend LRESULT CALLBACK cbtFilterHook(int code, WPARAM wParam, LPARAM lParam);
public:
	zxWnd();
	virtual ~zxWnd();
	void attach(HWND hWnd);
	void detach();
	void showWindow(bool show) const { ShowWindow(hWnd, show); }
	void updateWindow() const { UpdateWindow(hWnd); }
	void enableWindow(bool enable) const { EnableWindow(hWnd, enable); }
	bool isWindowVisible() const { return IsWindowVisible(hWnd); }

	static zxWnd* fromHWND(HWND hWnd);
	HWND getHWND() const { return hWnd; }
	LONG getStyles() const { return GetWindowLong(hWnd, GWL_STYLE); }
	virtual HWND create(LPCWSTR className, LPCWSTR windowName, DWORD dwStyle, int x, int y, int cx, int cy, HWND hWndParent, UINT nID, UINT nMenu);
	virtual HACCEL getHACCEL() { return hAccel; }
	virtual bool dialogMessage(MSG* pMsg) const { return false; }
	virtual bool translateAccelerator(MSG* pMsg) const { return TranslateAccelerator(hWnd, hAccel, pMsg); }
	HWND makeToolbar(WORD IDB, TBBUTTON* tbb, int cBitmaps, int cButtons, int cxButton, int cyButton, UINT nID);
protected:
	virtual BOOL onWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	virtual INT_PTR wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	virtual INT_PTR defWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual bool preCreate() { return true; }
	virtual void postCreate() {  }
	const SSH_MSGMAP_ENTRY* findMessageEntry(const SSH_MSGMAP_ENTRY* lpEntry, UINT nMsg, UINT nCode, UINT nID);
	ssh_cws registerClass(ssh_cws name, int idMenu, WNDPROC proc);
	HWND hWndParent;
	HWND hWnd, hWndToolbar;
	UINT wndID;
	UINT wmId;
	HBITMAP hBmp;
	HACCEL hAccel;
	WNDPROC pfnSuper;

	static const SSH_MSGMAP* PASCAL GetThisMessageMap();
	virtual const SSH_MSGMAP* GetMessageMap() const;
};

class zxDialog : public zxWnd {
	friend static INT_PTR CALLBACK DlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
public:
	zxDialog() : zxWnd(), nResult(0) {}
	virtual ~zxDialog() { }
	int create(WORD IDD, HWND hWndParent, bool modal);
	virtual bool dialogMessage(MSG* pMsg) const override { return IsDialogMessage(hWnd, pMsg); }
protected:
	void onClose();
	virtual void endDialog(int code) { nResult = code; EndDialog(hWnd, code); }
	virtual void onOK() { endDialog(IDOK); }
	virtual void onCancel() { endDialog(IDCANCEL); }
	virtual int onInitDialog(HWND hWnd) { return 1; }
		
	static const SSH_MSGMAP* PASCAL GetThisMessageMap();
	virtual const SSH_MSGMAP* GetMessageMap() const;

	int nResult;
};
