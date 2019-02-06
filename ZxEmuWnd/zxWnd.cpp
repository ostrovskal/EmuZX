
#include "stdafx.h"
#include "zxWnd.h"

struct THREAD_STATE {
	HHOOK hHookOldCbtFilter;
	zxWnd* wndInit;
};

static bool wndInit = false;
static zxWnd* wnds[256];
static THREAD_STATE thState;

const SSH_MSGMAP* zxWnd::GetMessageMap() const {
	return GetThisMessageMap(); 
}

const SSH_MSGMAP* PASCAL zxWnd::GetThisMessageMap() {
	static const SSH_MSGMAP_ENTRY _messageEntries[] = { {0, 0, 0, 0, 0, (SSH_PMSG)0 } };
	static const SSH_MSGMAP messageMap = { nullptr, &_messageEntries[0] };
	return &messageMap; 
}

zxWnd* zxWnd::fromHWND(HWND hWnd) {
	for(auto& w : wnds) {
		if(w == (zxWnd*)-1) continue;
		if(w->hWnd == hWnd) return w;
	}
	return nullptr;
}

void zxWnd::attach(HWND hWnd) {
	if(!fromHWND(hWnd)) {
		for(int i = 0 ; i < 256; i++) {
			auto w = wnds[i];
			if(w == (zxWnd*)-1) {
				wnds[i] = this;
				this->hWnd = hWnd;
				break;
			}
		}
	}
}

void zxWnd::detach() {
	for(auto w : wnds) {
		for(int i = 0; i < 256; i++) {
			auto w = wnds[i];
			if(w != this) continue;
			wnds[i] = (zxWnd*)-1;
			w->hWnd = nullptr;
			break;
		}
	}
}

static LRESULT CALLBACK MainWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	auto wnd = zxWnd::fromHWND(hWnd);
	if(wnd) return wnd->wndProc(hWnd, message, wParam, lParam);
	return DefWindowProc(hWnd, message, wParam, lParam);
}

static INT_PTR CALLBACK DlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	return MainWndProc(hWnd, message, wParam, lParam);
}

LRESULT CALLBACK cbtFilterHook(int code, WPARAM wParam, LPARAM lParam) {
	if(code == HCBT_CREATEWND) {
		LPCREATESTRUCT lpcs = ((LPCBT_CREATEWND)lParam)->lpcs;
		zxWnd* wndInit = thState.wndInit;
		if(wndInit) {
			HWND hWnd = (HWND)wParam;
			wndInit->attach(hWnd);
			SetWindowLongPtr(hWnd, GWLP_WNDPROC, (DWORD_PTR)MainWndProc);
			thState.wndInit = NULL;
		}
	}
	return CallNextHookEx(thState.hHookOldCbtFilter, code, wParam, lParam);
}

void HookWindowCreate(zxWnd* wnd) {
	if(thState.wndInit == wnd) return;
	if(thState.hHookOldCbtFilter == NULL) {
		thState.hHookOldCbtFilter = ::SetWindowsHookEx(WH_CBT, cbtFilterHook, NULL, ::GetCurrentThreadId());
	}
	thState.wndInit = wnd;
}

const SSH_MSGMAP_ENTRY* zxWnd::findMessageEntry(const SSH_MSGMAP_ENTRY* lpEntry, UINT nMsg, UINT nCode, UINT nID) {
	while(lpEntry->nSig != sshSig_end) {
		if(lpEntry->nMessage == nMsg && lpEntry->nCode == nCode && (nID >= lpEntry->nID && nID <= lpEntry->nLastID))
			return lpEntry;
		lpEntry++;
	}
	return nullptr;
}

INT_PTR zxWnd::wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	LRESULT lResult = 0;
	if(!onWndMsg(message, wParam, lParam, &lResult))
		lResult = DefWindowProc(hWnd, message, wParam, lParam);
	return lResult;
}

BOOL zxWnd::onWndMsg(UINT msg, WPARAM wParam, LPARAM lParam, LRESULT* pResult) {
	const SSH_MSGMAP_ENTRY* lpEntry;
	POINT pt;
	HANDLE handle;
	UINT nCode = 0;
	HWND hWndCtrl;
	SSH_NOTIFY notify;
	SSH_NOTIFY* pNotify = nullptr;
	LRESULT lResult = FALSE;

	wmId = 0;
	if(msg == WM_COMMAND) {
		wmId = LOWORD(wParam);
		nCode = HIWORD(wParam);
		hWndCtrl = (HWND)lParam;
		if(wmId == 0) return FALSE;
		if(hWndCtrl == NULL) nCode = CN_COMMAND;
	} else if(msg == WM_NOTIFY) {
		NMHDR* pNMHDR = (NMHDR*)lParam;
		hWndCtrl = pNMHDR->hwndFrom;
		wmId = LOWORD(wParam);
		nCode = pNMHDR->code;
		notify.pResult = &lResult;
		notify.pNMHDR = pNMHDR;
		pNotify = &notify;
	} else if(msg == WM_INITDIALOG) {
		onInitDialog(hWnd);
		*pResult = TRUE;
		return TRUE;
	}

	auto pMessageMap = GetMessageMap();
	for(; pMessageMap->pfnGetBaseMap != NULL; pMessageMap = (*pMessageMap->pfnGetBaseMap)()) {
		if(msg < 0xC000) {
			if((lpEntry = findMessageEntry(pMessageMap->lpEntries, msg, nCode, wmId))) {
				goto LDispatch;
			}
		} else {
			while((lpEntry = findMessageEntry(pMessageMap->lpEntries, 0xC000, 0, 0))) {
				UINT* pnID = (UINT*)(lpEntry->nSig);
				if(*pnID == msg) goto LDispatchRegistered;
				lpEntry++;
			}
		}
	}
	return FALSE;
LDispatch:
	MessageMapFunctions mmf;
	
	mmf.pfn = lpEntry->pfn;
	pt.x = (short)LOWORD(lParam);
	pt.y = (short)HIWORD(lParam);

	switch(lpEntry->nSig) {
		case sshSigCmd_v:
			(this->*mmf.pfnCmd_v_v)();
			break;
		case sshSigCmd_b:
			lResult = (this->*mmf.pfnCmd_b_v)();
			break;
		case sshSigNotify_v:
			(this->*mmf.pfnNotify_v_NMHDR_pl)(pNotify->pNMHDR, pNotify->pResult);
			break;
		case sshSigNotify_b: 
			lResult = (this->*mmf.pfnNotify_b_NMHDR_pl)(pNotify->pNMHDR, pNotify->pResult);
			break;
		case sshSigCmdUI: {
			/*
			CCmdUI* pCmdUI = (CCmdUI*)pExtra;
			(this->*mmf.pfnCmdUI_v_C)(pCmdUI);
			bResult = !pCmdUI->m_bContinueRouting;
			pCmdUI->m_bContinueRouting = FALSE;
			*/
			break;
		}
		case sshSigCmd_v_pv:
			(this->*mmf.pfnCmd_v_pv)(pNotify);
			break;
		case sshSigCmd_b_pv:
			lResult = (this->*mmf.pfnCmd_b_pv)(pNotify);
			break;
		case sshSig_l_p:
			lResult = (this->*mmf.pfn_l_p)(pt);
			break;
		case sshSig_b_D_v:
			lResult = (this->*mmf.pfn_b_D)(reinterpret_cast<HDC>(wParam));
			break;
		case sshSig_l_D_u:
			lResult = (this->*mmf.pfn_l_D_u)(reinterpret_cast<HDC>(wParam), (UINT)lParam);
			break;
		case sshSig_b_b_v:
			lResult = (this->*mmf.pfn_b_b)(static_cast<BOOL>(wParam));
			break;
		case sshSig_b_u_v:
			lResult = (this->*mmf.pfn_b_u)(static_cast<UINT>(wParam));
			break;
		case sshSig_b_h_v:
			lResult = (this->*mmf.pfn_b_h)(reinterpret_cast<HANDLE>(wParam));
			break;
		case sshSig_i_u_v:
			lResult = (this->*mmf.pfn_i_u)(static_cast<UINT>(wParam));
			break;
		case sshSig_C_v_v:
			lResult = reinterpret_cast<LRESULT>((this->*mmf.pfn_C_v)());
			break;
		case sshSig_v_u_W:
			(this->*mmf.pfn_v_u_W)(static_cast<UINT>(wParam), reinterpret_cast<HWND>(lParam));
			break;
		case sshSig_u_u_v:
			lResult = (this->*mmf.pfn_u_u)(static_cast<UINT>(wParam));
			break;
		case sshSig_b_v_v:
			lResult = (this->*mmf.pfn_b_v)();
			break;
		case sshSig_b_W_uu:
			lResult = (this->*mmf.pfn_b_W_u_u)(reinterpret_cast<HWND>(wParam), LOWORD(lParam), HIWORD(lParam));
			break;
		case sshSig_i_u_W_u:
			lResult = (this->*mmf.pfn_i_u_W_u)(LOWORD(wParam), reinterpret_cast<HWND>(lParam), HIWORD(wParam));
			break;
		case sshSig_i_uu_v:
			lResult = (this->*mmf.pfn_i_u_u)(LOWORD(wParam), HIWORD(wParam));
			break;
		case sshSig_i_W_uu:
			lResult = (this->*mmf.pfn_i_W_u_u)(reinterpret_cast<HWND>(wParam), LOWORD(lParam), HIWORD(lParam));
			break;
		case sshSig_i_v_s:
			lResult = (this->*mmf.pfn_i_s)(reinterpret_cast<LPTSTR>(lParam));
			break;
		case sshSig_i_v_S:
			lResult = (this->*mmf.pfn_i_S)(reinterpret_cast<LPCTSTR>(lParam));
			break;
		case sshSig_v_F_b:
			(this->*mmf.pfn_v_F_b)(reinterpret_cast<HFONT>(wParam), (BOOL)LOWORD(lParam));
			break;
		case sshSig_h_v_v:
			handle = (this->*mmf.pfn_h_v)();
			lResult = reinterpret_cast<LRESULT>(handle);
			break;
		case sshSig_h_b_h: 
			handle = (this->*mmf.pfn_h_b_h)(static_cast<BOOL>(wParam), reinterpret_cast<HANDLE>(lParam));
			lResult = reinterpret_cast<LRESULT>(handle);
			break;
		case sshSig_h_h_h:
			handle = (this->*mmf.pfn_h_h_h)(reinterpret_cast<HANDLE>(wParam), reinterpret_cast<HANDLE>(lParam));
			lResult = reinterpret_cast<LRESULT>(handle);
			break;
		case sshSig_l_w_l:
			lResult = (this->*mmf.pfn_l_w_l)(wParam, lParam);
			break;
		case sshSig_v_b_h:
			(this->*mmf.pfn_v_b_h)(static_cast<BOOL>(wParam), reinterpret_cast<HANDLE>(lParam));
			break;
		case sshSig_v_h_v:
			(this->*mmf.pfn_v_h)(reinterpret_cast<HANDLE>(wParam));
			break;
		case sshSig_v_h_h:
			(this->*mmf.pfn_v_h_h)(reinterpret_cast<HANDLE>(wParam), reinterpret_cast<HANDLE>(lParam));
			break;
		case sshSig_v_v_v:
			(this->*mmf.pfn_v_v)();
			break;
		case sshSig_v_u_v:
			(this->*mmf.pfn_v_u)(static_cast<UINT>(wParam));
			break;
		case sshSig_v_up_v:
			(this->*mmf.pfn_v_up)(static_cast<UINT_PTR>(wParam));
			break;
		case sshSig_v_u_u:
			(this->*mmf.pfn_v_u_u)(static_cast<UINT>(wParam), static_cast<UINT>(lParam));
			break;
		case sshSig_v_uu_v:
			(this->*mmf.pfn_v_u_u)(LOWORD(wParam), HIWORD(wParam));
			break;
		case sshSig_v_v_ii:
			(this->*mmf.pfn_v_i_i)(pt.x, pt.y);
			break;
		case sshSig_v_u_uu:
			(this->*mmf.pfn_v_u_u_u)(static_cast<UINT>(wParam), LOWORD(lParam), HIWORD(lParam));
			break;
		case sshSig_v_u_ii:
			(this->*mmf.pfn_v_u_i_i)(static_cast<UINT>(wParam), LOWORD(lParam), HIWORD(lParam));
			break;
		case sshSig_v_w_l:
			(this->*mmf.pfn_v_w_l)(wParam, lParam);
			break;
		case sshSig_v_D_v:
			(this->*mmf.pfn_v_D)(reinterpret_cast<HDC>(wParam));
			break;
		case sshSig_v_W_v:
			(this->*mmf.pfn_v_W)(reinterpret_cast<HWND>(wParam));
			break;
		case sshSig_v_v_W:
			(this->*mmf.pfn_v_W)(reinterpret_cast<HWND>(lParam));
			break;
		case sshSig_v_W_uu:
			(this->*mmf.pfn_v_W_u_u)(reinterpret_cast<HWND>(wParam), LOWORD(lParam), HIWORD(lParam));
			break;
		case sshSig_v_W_p:
			(this->*mmf.pfn_v_W_p)(reinterpret_cast<HWND>(wParam), pt);
			break;
		case sshSig_v_W_h:
			(this->*mmf.pfn_v_W_h)(reinterpret_cast<HWND>(wParam), reinterpret_cast<HANDLE>(lParam));
			break;
		case sshSig_ACTIVATE:
			(this->*mmf.pfn_v_u_W_b)(LOWORD(wParam), reinterpret_cast<HWND>(lParam), HIWORD(wParam));
			break;
		case sshSig_SCROLL:
			(this->*mmf.pfn_v_u_u_W)((short)LOWORD(wParam), (short)HIWORD(wParam), reinterpret_cast<HWND>(lParam));
			break;
		case sshSig_v_v_s:
			(this->*mmf.pfn_v_s)(reinterpret_cast<LPTSTR>(lParam));
			break;
		case sshSig_v_u_cs:
			(this->*mmf.pfn_v_u_cs)(static_cast<UINT>(wParam), reinterpret_cast<LPCTSTR>(lParam));
			break;
		case sshSig_OWNERDRAW:
			(this->*mmf.pfn_v_i_s)(static_cast<int>(wParam), reinterpret_cast<LPTSTR>(lParam));
			lResult = TRUE;
			break;
		case sshSig_i_i_s:
			lResult = (this->*mmf.pfn_i_i_s)(static_cast<int>(wParam), reinterpret_cast<LPTSTR>(lParam));
			break;
		case sshSig_u_v_p:
			lResult = (this->*mmf.pfn_u_p)(pt);
			break;
		case sshSig_u_v_v:
			lResult = (this->*mmf.pfn_u_v)();
			break;
		case sshSig_v_b_NCCALCSIZEPARAMS:
			(this->*mmf.pfn_v_b_NCCALCSIZEPARAMS)(static_cast<BOOL>(wParam), reinterpret_cast<NCCALCSIZE_PARAMS*>(lParam));
			break;
		case sshSig_v_v_WINDOWPOS:
			(this->*mmf.pfn_v_v_WINDOWPOS)(reinterpret_cast<WINDOWPOS*>(lParam));
			break;
		case sshSig_v_uu_M:
			(this->*mmf.pfn_v_u_u_M)(LOWORD(wParam), HIWORD(wParam), reinterpret_cast<HMENU>(lParam));
			break;
		case sshSig_v_u_p:
			(this->*mmf.pfn_v_u_p)(static_cast<UINT>(wParam), pt);
			break;
		case sshSig_SIZING:
			(this->*mmf.pfn_v_u_pr)(static_cast<UINT>(wParam), reinterpret_cast<LPRECT>(lParam));
			lResult = TRUE;
			break;
		case sshSig_MOUSEWHEEL:
			lResult = (this->*mmf.pfn_b_u_s_p)(LOWORD(wParam), (short)HIWORD(wParam), pt);
			if(!lResult) return FALSE;
			break;
		case sshSig_MOUSEHWHEEL:
			(this->*mmf.pfn_MOUSEHWHEEL)(LOWORD(wParam), (short)HIWORD(wParam), pt);
			break;
		case sshSig_l:
			lResult = (this->*mmf.pfn_l_v)();
			if(lResult != 0) return FALSE;
			break;
		case sshSig_u_W_u:
			lResult = (this->*mmf.pfn_u_W_u)(reinterpret_cast<HWND>(wParam), static_cast<UINT>(lParam));
			break;
		case sshSig_APPCOMMAND:
			(this->*mmf.pfn_APPCOMMAND)(reinterpret_cast<HWND>(wParam), static_cast<UINT>(GET_APPCOMMAND_LPARAM(lParam)), 
										static_cast<UINT>(GET_DEVICE_LPARAM(lParam)), static_cast<UINT>(GET_KEYSTATE_LPARAM(lParam)));
			lResult = TRUE;
			break;
		case sshSig_u_u_u:
			lResult = (this->*mmf.pfn_u_u_u)(static_cast<UINT>(wParam), static_cast<UINT>(lParam));
			break;
		case sshSig_u_u_l:
			lResult = (this->*mmf.pfn_u_u_l)(static_cast<UINT>(wParam), lParam);
			break;
		case sshSig_MOUSE_XBUTTON:
			(this->*mmf.pfn_MOUSE_XBUTTON)(static_cast<UINT>(GET_KEYSTATE_WPARAM(wParam)), static_cast<UINT>(GET_XBUTTON_WPARAM(wParam)), pt);
			lResult = TRUE;
			break;
		case sshSig_MOUSE_NCXBUTTON:
			(this->*mmf.pfn_MOUSE_NCXBUTTON)(static_cast<short>(GET_NCHITTEST_WPARAM(wParam)), static_cast<UINT>(GET_XBUTTON_WPARAM(wParam)), pt);
			lResult = TRUE;
			break;
		case sshSig_b_v_ii:
			lResult = (this->*mmf.pfn_b_v_ii)(pt.x, pt.y);
			break;
	}
	goto LReturnTrue;
LDispatchRegistered:
	mmf.pfn = lpEntry->pfn;
	lResult = (this->*mmf.pfn_l_w_l)(wParam, lParam);
LReturnTrue:
	if(pResult) *pResult = lResult;
	return TRUE;
}

zxWnd::zxWnd() : wndID(0), hBmp(nullptr),  hWnd(nullptr), hWndToolbar(nullptr), parent(nullptr) {
	if(!wndInit) {
		memset(wnds, -1, sizeof(wnds));
		wndInit = true;
	}
}

zxWnd::~zxWnd() {
	detach();
	DeleteObject(hBmp);
	if(hWndToolbar) {
		DestroyWindow(hWndToolbar);
		hWndToolbar = nullptr;
	}
}

ssh_cws zxWnd::registerClass(ssh_cws name, int idMenu, WNDPROC proc) {
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	wcex.lpfnWndProc = proc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInst;
	wcex.hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_ZXEMUWND));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = (idMenu == 0 ? nullptr : MAKEINTRESOURCEW(idMenu));
	wcex.lpszClassName = name;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	RegisterClassExW(&wcex);

	return name;
}

HWND zxWnd::create(LPCWSTR className, LPCWSTR windowName, DWORD dwStyle, int x, int y, int cx, int cy, zxWnd* wndParent, UINT nID, UINT nMenu) {
	if(hWnd) return 0;
	if(!preCreate()) return 0;
	HookWindowCreate(this);
	if(!(hWnd = CreateWindowW(registerClass(className, nMenu, MainWndProc), windowName, dwStyle, 
							  x, y, cx, cy, (wndParent ? wndParent->hWnd : nullptr), nullptr, hInst, nullptr)))
		return (HWND)0;
	wndID = nID;
	postCreate();
	parent = wndParent;
	return hWnd;
}

HWND zxWnd::makeToolbar(WORD IDB, TBBUTTON* tbb, int cBitmaps, int cButtons, int cxButton, int cyButton, UINT nID) {
	hBmp = LoadBitmap(hInst, (LPCWSTR)IDB);
	if((hWndToolbar = CreateToolbarEx(hWnd, TBSTYLE_FLAT | TBSTYLE_TOOLTIPS | WS_CHILD | WS_VISIBLE | CCS_TOP | WS_BORDER, nID,
							   cBitmaps, nullptr, (UINT_PTR)hBmp, tbb, cButtons, cxButton, cyButton, cxButton * cBitmaps, cyButton,
							   sizeof(TBBUTTON)))) {
		SendMessage(hWndToolbar, TB_SETEXTENDEDSTYLE, 0, TBSTYLE_EX_DRAWDDARROWS);
		SendMessage(hWndToolbar, TB_SETMAXTEXTROWS, 0, 0);
		UpdateWindow(hWndToolbar);
	}
	return hWndToolbar;
}

int zxDialog::create(WORD IDD, WORD IDA, zxWnd* wndParent, bool modal) {
	if(!preCreate()) return IDCANCEL;
	HookWindowCreate(this);
	if((hWnd = CreateDialog(hInst, MAKEINTRESOURCE(IDD), (wndParent ? wndParent->getHWND() : nullptr), DlgProc))) {
		parent = wndParent;
		postCreate();
		if(modal) {
			ShowWindow(hWnd, true);
			UpdateWindow(hWnd);
			nResult = 0;
			HACCEL hAccelTable = LoadAccelerators(hInst, MAKEINTRESOURCE(IDA));
			MSG msg;
			while(!nResult) {
				do {
					if(!SshMsgPump(&msg, hAccelTable)) break;
				} while(::PeekMessage(&msg, NULL, NULL, NULL, PM_NOREMOVE));
			}
			DestroyWindow(hWnd);
			hWnd = nullptr;
			return nResult;
		}
	}
	return IDOK;
}
