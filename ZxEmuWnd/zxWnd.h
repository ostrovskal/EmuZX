
#pragma once

struct SSH_MSGMAP;
struct SSH_MSGMAP_ENTRY;
class zxWnd {
	friend LRESULT CALLBACK MainWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
public:
	zxWnd();
	virtual ~zxWnd();
	void attach(HWND hWnd);
	void detach();
	static zxWnd* fromHWND(HWND hWnd);
	HWND getHWND() const { return hWnd; }
	LONG getStyles() const { return GetWindowLong(hWnd, GWL_STYLE); }
	operator const HWND() { return hWnd; }
	virtual HWND create(LPCWSTR className, LPCWSTR windowName, DWORD dwStyle, int x, int y, int cx, int cy, zxWnd* wndParent, UINT nID, UINT nMenu);
	HWND makeToolbar(WORD IDB, TBBUTTON* tbb, int cBitmaps, int cButtons, int cxButton, int cyButton, UINT nID);
protected:
	virtual BOOL onWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	virtual INT_PTR wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	virtual int onInitDialog(HWND hWnd) { return 1; }
	virtual bool preCreate() { return true; }
	virtual void postCreate() {  }
	const SSH_MSGMAP_ENTRY* findMessageEntry(const SSH_MSGMAP_ENTRY* lpEntry, UINT nMsg, UINT nCode, UINT nID);
	ssh_cws registerClass(ssh_cws name, int idMenu, WNDPROC proc);
	zxWnd* parent;
	HWND hWnd, hWndToolbar;
	UINT wndID;
	UINT wmId;
	HBITMAP hBmp;

	static const SSH_MSGMAP* PASCAL GetThisMessageMap();
	virtual const SSH_MSGMAP* GetMessageMap() const;
};

class zxDialog : public zxWnd {
public:
	zxDialog() : zxWnd(), nResult(0) {}
	virtual ~zxDialog() { }
	int create(WORD IDD, WORD IDA, zxWnd* wndParent, bool modal);
protected:
	virtual bool onOK() { return true; }
	virtual bool onCancel() { return true; }
	int nResult;
};
