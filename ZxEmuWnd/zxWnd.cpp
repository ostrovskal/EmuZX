
#include "stdafx.h"
#include "zxWnd.h"

struct THREAD_STATE {
	HHOOK hHookOldCbtFilter;
	zxWnd* wndInit;
};

static bool wndInit = false;
static zxWnd* wnds[256];
static THREAD_STATE thState;

//BEGIN_MSG_MAP(zxWnd, zxWnd)
//END_MSG_MAP()

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
		if(wndInit || (!(lpcs->style & WS_CHILD))) {
			HWND hWnd = (HWND)wParam;
			if(wndInit) {
				wndInit->attach(hWnd);
				SetWindowLongPtr(hWnd, GWLP_WNDPROC, (DWORD_PTR)MainWndProc);
				thState.wndInit = NULL;
			}
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

INT_PTR zxWnd::wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch(message) {
		case WM_NCCALCSIZE: break;
		case WM_PAINT: if(onPaint()) return 0; break;
		case WM_CREATE: return (onCreate((LPCREATESTRUCT)lParam) ? 0 : -1);
		case WM_SIZE: if(onSize(wParam, LOWORD(lParam), HIWORD(lParam))) return 0; break;
		case WM_CLOSE: if(!onClose()) return 0; break;
		case WM_SETFONT: onFont((HFONT)wParam, (bool)lParam); break;
		case WM_DESTROY: onDestroy(); if(this == &theApp) { PostQuitMessage(0); return 1; } break;
		case WM_COMMAND: if(onCommand(LOWORD(wParam), HIWORD(wParam), lParam)) return 0; break;
		case WM_ERASEBKGND: if(hWndToolbar) SendMessage(hWndToolbar, message, wParam, lParam); return 1;
		case WM_KEYDOWN: if(onKey((int)wParam, lParam, true)) return 0; break;
		case WM_KEYUP: if(onKey((int)wParam, lParam, false)) return 0; break;
		case WM_NOTIFY: if(onNotify((LPNMHDR)lParam)) return 0; break;
		case WM_DRAWITEM: if(onDrawItem((UINT)wParam, (LPDRAWITEMSTRUCT)lParam)) return 1; break;
		case WM_VSCROLL: if(onScroll(HIWORD(wParam), LOWORD(wParam), (HWND)lParam, SB_VERT)) return 0; break;
		case WM_HSCROLL: if(onScroll(HIWORD(wParam), LOWORD(wParam), (HWND)lParam, SB_HORZ)) return 0; break;
		case WM_MOUSEWHEEL: if(onMouseWheel(GET_KEYSTATE_WPARAM(wParam), ((short)LOWORD(lParam)),
											((short)HIWORD(lParam)), GET_WHEEL_DELTA_WPARAM(wParam))) return 0;  break;
		case WM_MOUSEMOVE: if(onMouseMove(GET_KEYSTATE_WPARAM(wParam),
											((short)LOWORD(lParam)), ((short)HIWORD(lParam)))) return 0; break;
		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN: if(onMouseButtonDown(GET_KEYSTATE_WPARAM(wParam), ((short)LOWORD(lParam)),
													((short)HIWORD(lParam)), message == WM_LBUTTONDOWN)) return 0; break;
		case WM_LBUTTONUP: 
		case WM_RBUTTONUP: if(onMouseButtonUp(GET_KEYSTATE_WPARAM(wParam), ((short)LOWORD(lParam)), 
												((short)HIWORD(lParam)), message == WM_LBUTTONUP)) return 0; break;
		case WM_LBUTTONDBLCLK:
		case WM_RBUTTONDBLCLK: if(onMouseButtonDblClk(GET_KEYSTATE_WPARAM(wParam), ((short)LOWORD(lParam)),
													((short)HIWORD(lParam)), message == WM_LBUTTONDBLCLK)) return 0; break;
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}

INT_PTR zxDialog::wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch(message) {
		case WM_INITDIALOG: onInitDialog(hWnd, lParam); return 1;
		case WM_ERASEBKGND: if(hWndToolbar) SendMessage(hWndToolbar, message, wParam, lParam); return 0;
		case WM_COMMAND: {
			int wmId = LOWORD(wParam);
			auto res = onCommand(wmId, HIWORD(wParam), lParam);
			if((res && wmId == IDOK) || wmId == IDCANCEL) {
				SendMsg(WM_CLOSE, 0, 0);
				nResult = wmId;
				res = true;
			}
			return !res;
		}
		case WM_CTLCOLORSTATIC: return onCtlColorStatic((HDC)wParam, (HWND)lParam);
	}
	return zxWnd::wndProc(hWnd, message, wParam, lParam);
}

zxWnd::zxWnd() : wndID(0), hWnd(nullptr), hWndToolbar(nullptr), parent(nullptr), hBmp(nullptr) {
	if(!wndInit) {
		memset(wnds, -1, sizeof(wnds));
		wndInit = true;
	}
}

zxWnd::~zxWnd() {
	detach();
	DeleteObject(hBmp); hBmp = nullptr;
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
	if(!preCreate()) return false;
	HookWindowCreate(this);
	if(!(hWnd = CreateWindowW(registerClass(className, nMenu, MainWndProc), windowName, dwStyle, 
							  x, y, cx, cy, (wndParent ? wndParent->hWnd : nullptr), nullptr, hInst, nullptr)))
		return (HWND)0;
	wndID = nID;
	postCreate();
	parent = wndParent;
	return hWnd;
}

zxDialog::~zxDialog() {
	DestroyWindow(hWnd);
//	EndDialog(hWnd, 0);
}

bool zxWnd::makeToolbar(WORD IDB, TBBUTTON* tbb, int cBitmaps, int cButtons, int cxButton, int cyButton) {
	if(hWndToolbar) return true;
	hBmp = LoadBitmap(hInst, (LPCWSTR)IDB);
	if((hWndToolbar = CreateToolbarEx(hWnd, TBSTYLE_FLAT | TBSTYLE_TOOLTIPS | WS_CLIPSIBLINGS | WS_CHILD | WS_VISIBLE | CCS_TOP, -1,
									  cBitmaps, nullptr, (UINT_PTR)hBmp, tbb, cButtons, cxButton, cyButton, cxButton * cBitmaps, cyButton,
									  sizeof(TBBUTTON)))) {
		SendMessage(hWndToolbar, TB_SETEXTENDEDSTYLE, 0, TBSTYLE_EX_DRAWDDARROWS);
		SendMessage(hWndToolbar, TB_SETMAXTEXTROWS, 0, 0);
		UpdateWindow(hWndToolbar);
		return true;
	}
	return false;

}

int zxDialog::create(WORD IDD, zxWnd* wndParent, bool modal) {
	if(!preCreate()) return 2;
	HookWindowCreate(this);
	if((hWnd = CreateDialog(hInst, MAKEINTRESOURCE(IDD), (wndParent ? wndParent->getHWND() : nullptr), DlgProc))) {
		parent = wndParent;
		postCreate();
		if(modal) {
			showWindow(true);
			updateWindow();
			nResult = 0;
			HACCEL hAccelTable = LoadAccelerators(hInst, MAKEINTRESOURCE(IDC_ZXEMUWND));
			MSG msg;
			while(!nResult) {
				while(!::PeekMessage(&msg, NULL, NULL, NULL, PM_NOREMOVE)) {
					// функция для работы пока нет сообщений
					//SendMsg(WM_IDLE, 0, 0);
				}
				if(!::GetMessage(&msg, hWnd, NULL, NULL)) break;
				if(!TranslateAccelerator(hWnd, hAccelTable, &msg)) {
					if(!IsDialogMessage(hWnd, &msg)) {
						TranslateMessage(&msg);
						DispatchMessage(&msg);
					}
				}
			}
		}
		return nResult;
	}
	return 1;
}
