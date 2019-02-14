
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
	virtual HWND create(LPCWSTR className, LPCWSTR windowName, DWORD dwStyle, int x, int y, int cx, int cy, zxWnd* wndParent, UINT nID, UINT nMenu);
	virtual HACCEL getHACCEL() { return hAccel; }
	virtual bool dialogMessage(MSG* pMsg) const { return false; }
	virtual bool translateAccelerator(MSG* pMsg) const { return TranslateAccelerator(hWnd, hAccel, pMsg); }
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
	HACCEL hAccel;

	static const SSH_MSGMAP* PASCAL GetThisMessageMap();
	virtual const SSH_MSGMAP* GetMessageMap() const;
};

struct _AFX_OCC_DIALOG_INFO {
	DLGTEMPLATE* m_pNewTemplate;
	DLGITEMTEMPLATE** m_ppOleDlgItems;

	unsigned m_cItems;
	struct ItemInfo {
		unsigned nId;
		BOOL bAutoRadioButton;
	};
	ItemInfo *m_pItemInfo;
};

class zxDialog : public zxWnd {
public:
	zxDialog() : zxWnd(), nResult(0) {}
	virtual ~zxDialog() { }
	int create(WORD IDD, zxWnd* wndParent, bool modal);
	virtual bool dialogMessage(MSG* pMsg) const override { return IsDialogMessage(hWnd, pMsg); }
protected:
	virtual void endDialog(int code) { nResult = code; EndDialog(hWnd, code); }
	virtual void onOK() { endDialog(IDOK); }
	virtual void onCancel() { endDialog(IDCANCEL); }
	BOOL CreateDlgIndirect(LPCDLGTEMPLATE lpDialogTemplate, zxWnd* pParentWnd, HINSTANCE hInst);
	const DLGTEMPLATE* preCreateDialog(_AFX_OCC_DIALOG_INFO* pDlgInfo, const DLGTEMPLATE* pOrigTemplate);
	DLGTEMPLATE* splitDialogTemplate(const DLGTEMPLATE* pTemplate, DLGITEMTEMPLATE** ppOleDlgItems);
	void postCreateDialog(_AFX_OCC_DIALOG_INFO* pDlgInfo);
	int nResult;
	static const SSH_MSGMAP* PASCAL GetThisMessageMap();
	virtual const SSH_MSGMAP* GetMessageMap() const;
};
