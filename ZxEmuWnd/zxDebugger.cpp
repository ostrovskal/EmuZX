
#include "stdafx.h"
#include "resource.h"
#include "zxDebugger.h"
#include "zxDisAsm.h"
#include "SettingsZX.h"

HWND hWndDebugger = nullptr;
static HWND hWndDlg = nullptr;
static ssh_w _FAKE = 0;

ZX_DEBUGGER dlgElems[] = {
	{IDC_LIST_DA, IDC_LIST_DA, nullptr, &_FAKE, 0xffff},
	{IDC_LIST_STACK, IDC_LIST_STACK, nullptr, &_FAKE, 0xffff},
	{IDC_EDIT_BC, IDC_STATIC_BC, nullptr, (ssh_w*)&regsZX[RC], 0xffff},
	{IDC_EDIT_DE, IDC_STATIC_DE, nullptr, (ssh_w*)&regsZX[RE], 0xffff},
	{IDC_EDIT_HL, IDC_STATIC_HL, nullptr, (ssh_w*)&regsZX[RL], 0xffff},
	{IDC_EDIT_AF, IDC_STATIC_AF, nullptr, (ssh_w*)&regsZX[RA], 0xffff},
	{IDC_EDIT_BC_, IDC_STATIC_BC_, nullptr, (ssh_w*)&regsZX[RC + 14], 0xffff},
	{IDC_EDIT_DE_, IDC_STATIC_DE_, nullptr, (ssh_w*)&regsZX[RE + 14], 0xffff},
	{IDC_EDIT_HL_, IDC_STATIC_HL_, nullptr, (ssh_w*)&regsZX[RL + 14], 0xffff},
	{IDC_EDIT_AF_, IDC_STATIC_AF_, nullptr, (ssh_w*)&regsZX[RA + 14], 0xffff},
	{IDC_EDIT_SP, IDC_STATIC_SP, nullptr, (ssh_w*)&regsZX[RSPL], 0xffff},
	{IDC_EDIT_IX, IDC_STATIC_IX, nullptr, (ssh_w*)&regsZX[RIXL], 0xffff},
	{IDC_EDIT_IY, IDC_STATIC_IY, nullptr, (ssh_w*)&regsZX[RIYL], 0xffff},
	{IDC_EDIT_PC, IDC_STATIC_PC, nullptr, &_PC, 0xffff},
	{IDC_EDIT_IFF1, IDC_STATIC_IFF1, &regsZX[RIFF1], nullptr, 1},
	{IDC_EDIT_IFF2, IDC_STATIC_IFF2, &regsZX[RIFF2], nullptr, 1},
	{IDC_EDIT_IM, IDC_STATIC_IM, &regsZX[RIM], nullptr, 0x3},
	{IDC_EDIT_I, IDC_STATIC_I, &regsZX[RI], nullptr, 0xff},
	{IDC_EDIT_R, IDC_STATIC_R, &regsZX[RR], nullptr, 0xff},
	{IDC_EDIT_S, IDC_STATIC_S, &regsZX[RF], nullptr, FS, 7},
	{IDC_EDIT_Z, IDC_STATIC_Z, &regsZX[RF], nullptr, FZ, 6},
	{IDC_EDIT_5, IDC_STATIC_5, &regsZX[RF], nullptr, F5, 5},
	{IDC_EDIT_H, IDC_STATIC_H, &regsZX[RF], nullptr, FH, 4},
	{IDC_EDIT_3, IDC_STATIC_3, &regsZX[RF], nullptr, F3, 3},
	{IDC_EDIT_P, IDC_STATIC_P, &regsZX[RF], nullptr, FPV,2},
	{IDC_EDIT_N, IDC_STATIC_N, &regsZX[RF], nullptr, FN, 1},
	{IDC_EDIT_C, IDC_STATIC_C, &regsZX[RF], nullptr, FC, 0},
	{IDC_EDIT_TSTATE1, IDC_STATIC_TSTATE, nullptr, (ssh_w*)&_TSTATE, 0xffff}
};

static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	return theApp.debug->procWnd(hWnd, message, wParam, lParam);
}

INT_PTR zxDebugger::procWnd(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch(msg) {
		case WM_ERASEBKGND: return 1;
		case WM_CLOSE: 
			modifyTSTATE(ZX_EXEC, ZX_DEBUG);
			theApp.changeExecute(false);
			theApp.changeWndDebugger(true);
			return 0;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

static INT_PTR CALLBACK DlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	return theApp.debug->procDlg(hWnd, msg, wParam, lParam);
}

zxDebugger::zxDebugger() : da(nullptr), hFont(nullptr), hWndToolbar(nullptr) {
	memset(bps, -1, sizeof(bps));
	hFont = CreateFont(-12, 0, 0, 0, FW_THIN, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_TT_PRECIS,
					   CLIP_DEFAULT_PRECIS, PROOF_QUALITY, FF_ROMAN, L"Courier New");

	hBmp = LoadBitmap(hInst, (LPCWSTR)IDB_TOOLBAR_DEBUGGER);
	hbrSel = CreateSolidBrush(GetSysColor(COLOR_HIGHLIGHT));
	hbrUnSel = CreateSolidBrush(GetSysColor(COLOR_WINDOW));

	if(!(hWndDebugger = CreateWindowW(theApp.registerClass(L"DebuggerZX", 0, WndProc), L"Debugger", WS_CLIPCHILDREN | WS_CAPTION | WS_SYSMENU,
								  CW_USEDEFAULT, CW_USEDEFAULT, 580, 595, hWnd, nullptr, hInst, nullptr))) return;

	da = new zxDisAsm;
	// прочитать точки останова из установок
	int count = 0;
	for(int i = OPT_BPS0; i <= OPT_BPS9; i++) {
		auto opt = theApp.getOpt(i);
		if(opt->sval.is_empty()) continue;
		auto vals = opt->sval.split(L",", count);
		if(count != 4) {
			auto bp = bps[i - OPT_BPS0];
			bp->address1 = _wtoi(vals[0]);
			bp->address2 = _wtoi(vals[1]);
			bp->value = _wtoi(vals[2]);
			bp->flags = _wtoi(vals[3]);
		}
		delete vals;
	}
}

zxDebugger::~zxDebugger() {
	SAFE_DELETE(da);
	DeleteObject(hFont);
	DeleteObject(hbrSel);
	DeleteObject(hbrUnSel);
	DeleteObject(hBmp);
}

void zxDebugger::makeToolbar() {
	TBBUTTON tbb[] = {
		{0, IDM_PREV_BREAKPOINT, TBSTATE_ENABLED, BTNS_AUTOSIZE,{0}, 0, (INT_PTR)L"Previous Breakpoint\tCtr+F9"},
		{1, IDM_BREAKPOINT, TBSTATE_ENABLED, BTNS_AUTOSIZE,{0}, 0, (INT_PTR)L"Установить/Удалить точку останова\tF9"},
		{2, IDM_NEXT_BREAKPOINT, TBSTATE_ENABLED, BTNS_AUTOSIZE,{0}, 0, (INT_PTR)L"Next Breakpoint\tShift+F9"},
		{3, IDM_LIST_BREAKPOINTS, TBSTATE_ENABLED, BTNS_AUTOSIZE,{0}, 0, (INT_PTR)L"Список точек останова\tF8"},
		{10, -1, TBSTATE_ENABLED, BTNS_SEP, {0}, 0, 0},
		{4, IDM_STEP_INFO, TBSTATE_ENABLED, BTNS_AUTOSIZE,{0}, 0, (INT_PTR)L"Шаг с заходом\tF10"},
		{5, IDM_STEP_OVER, TBSTATE_ENABLED, BTNS_AUTOSIZE,{0}, 0, (INT_PTR)L"Шаг с обходом\tF11"},
		{10, -1, TBSTATE_ENABLED, BTNS_SEP, {0}, 0, 0},
		{6, IDM_PAUSE, 0, BTNS_AUTOSIZE,{0}, 0, (INT_PTR)L"Приостановить\tF4"},
		{7, IDM_RUN, TBSTATE_ENABLED, BTNS_AUTOSIZE,{0}, 0, (INT_PTR)L"Запустить\tF5"},
		{10, -1, TBSTATE_ENABLED, BTNS_SEP, {0}, 0, 0},
		{8, IDM_UNDO, 0, BTNS_AUTOSIZE,{0}, 0, (INT_PTR)L"Undo\tCtrl+Z"},
		{9, IDM_REDO, 0, BTNS_AUTOSIZE,{0}, 0, (INT_PTR)L"Redo\tCtrl+Y"},
		{10, -1, TBSTATE_ENABLED, BTNS_SEP,{0}, 0, 0},
		{10, IDM_HEX_DEC, TBSTATE_ENABLED, BTNS_AUTOSIZE,{0}, 0, (INT_PTR)L"Hex/Dec"},
		{10, -1, TBSTATE_ENABLED, BTNS_SEP, {0}, 0, 0},
		{11, IDM_OVER_PROC, TBSTATE_ENABLED, BTNS_AUTOSIZE,{0}, 0, (INT_PTR)L"Шаг с выходом\tShift+F11"}
	};
	if(!hWndToolbar) {
		hWndToolbar = CreateToolbarEx(hWndDlg, TBSTYLE_FLAT | TBSTYLE_TOOLTIPS | WS_CLIPSIBLINGS | WS_CHILD | WS_VISIBLE | CCS_TOP, -1, 15, NULL, (UINT_PTR)hBmp, tbb, 20, 16, 16, 240, 16, sizeof(TBBUTTON));
		SendMessage(hWndToolbar, TB_SETEXTENDEDSTYLE, 0, TBSTYLE_EX_DRAWDDARROWS);
		SendMessage(hWndToolbar, TB_SETMAXTEXTROWS, 0, 0);
	}
}

void zxDebugger::show(bool visible) {
	if(visible) {
		if(!hWndDlg) {
			if(!(hWndDlg = CreateDialog(hInst, MAKEINTRESOURCE(IDD_DIALOG_DEBUGGER), hWndDebugger, DlgProc)))
				return;
			makeToolbar();
		}
		modifyTSTATE(ZX_DEBUG, ZX_EXEC);
		theApp.changeExecute(false);

		ShowWindow(hWndDlg, true);
		ShowWindow(hWndDebugger, true);
		UpdateWindow(hWndDebugger);
		updateHexDec(false);
		updateRegisters(_PC, true);
	} else {
		ShowWindow(hWndDebugger, false);
		modifyTSTATE(ZX_EXEC, ZX_DEBUG);
		theApp.changeExecute(false);
		// записать точки останова в установки
		for(int i = OPT_BPS0; i <= OPT_BPS9; i++) {
			auto bp = bps[i - OPT_BPS0];
			swprintf_s(tmpBuf, MAX_PATH, L"%d,%d,%d,%d", bp->address1, bp->address2, bp->value, bp->flags);
			theApp.getOpt(i)->sval = tmpBuf;
		}
	}
}

void zxDebugger::updateRegisters(int newPC, bool always) {
	bool decimal = theApp.getOpt(OPT_DECIMAL)->dval;
	for(auto& zx : dlgElems) {
		ssh_w val = (((zx.regb == nullptr) ? *zx.regw : (ssh_w)(*zx.regb)) & zx.msk) >> zx.shift;
		zx.change = zx.val != val;
		if(zx.change || always) {
			zx.val = val;
			auto fmt = (zx.msk < 255 ? L"%d" : (zx.regb ? radix[decimal] : radix[decimal + 18]));
			SetWindowText(zx.hWndMain, fromNum(val, fmt));
		}
	}
	if(newPC != -1) {
		da->decode(newPC, 34);
		firstPC = newPC;
		InvalidateRect(dlgElems[0].hWndMain, nullptr, true);
		UpdateWindow(dlgElems[0].hWndMain);
	}
	InvalidateRect(dlgElems[1].hWndMain, nullptr, true);
	UpdateWindow(dlgElems[1].hWndMain);
	InvalidateRect(hWndDlg, nullptr, true);
	UpdateWindow(hWndDlg);
}

void zxDebugger::updateHexDec(bool change) {
	auto opt = theApp.getOpt(OPT_DECIMAL);
	if(change) {
		opt->dval = !opt->dval;
		updateRegisters(firstPC, true);
	}
	SendMessage(hWndToolbar, TB_SETSTATE, IDM_HEX_DEC, TBSTATE_ENABLED | opt->dval);
}

void zxDebugger::init(HWND hWnd) {
	HWND h;

	theApp.changeExecute(false);

	for(auto& zx : dlgElems) {
		zx.hWndMain = GetDlgItem(hWnd, zx.idMain);
		zx.hWndText = GetDlgItem(hWnd, zx.idText);
		zx.val = (((zx.regb == nullptr) ? *zx.regw : (ssh_w)*zx.regb) & zx.msk) >> zx.shift;
	}
	h = dlgElems[0].hWndMain;
	SendMessage(h, WM_SETFONT, WPARAM(hFont), TRUE);
	SendMessage(h, LB_SETITEMHEIGHT, 0, 16);
	SendMessage(h, LB_SETCOUNT, 65536, 0);
	SendMessage(h, LB_SETTOPINDEX, _PC, 0);
	h = dlgElems[1].hWndMain;
	SendMessage(h, WM_SETFONT, WPARAM(hFont), TRUE);
	SendMessage(h, LB_SETITEMHEIGHT, 0, 16);
	SendMessage(h, LB_SETCOUNT, 32768, 0);
	firstPC = _PC;
	topIndexDA = _PC;
	updateStack(*_SP);

	GetWindowRect(hWndDebugger, &rcWnd);
}

void zxDebugger::updateStack(int sp) {
	sp -= 10;
	if(sp < 0) sp = 0;
	else if((sp + 20) > 65535) sp = 65515;
	topIndexSP = sp / 2;
	SendMessage(dlgElems[1].hWndMain, LB_SETTOPINDEX, topIndexSP, 0);
}

void zxDebugger::drawItem(DRAWITEMSTRUCT* dis, int id) {
	StringZX txt;

	auto dec = theApp.getOpt(OPT_DECIMAL)->dval;
	auto item = dis->itemID;
	if(id == dlgElems[0].idMain) {
		auto top = SendMessage(dis->hwndItem, LB_GETTOPINDEX, 0, 0);
		if(top != topIndexDA) {
			bool dir = top > topIndexDA;
			auto pc = da->move(firstPC, dir, dir ? (int)(top - topIndexDA) : (int)(topIndexDA - top));
			topIndexDA = (int)top;
			updateRegisters(pc, false);
		}
		ssh_w idx = (ssh_w)(item - top);
		if(idx < 40)
			txt = da->makeCommand((int)idx, DA_FCODE | DA_FADDR | DA_FPADDR | (int)dec);
	} else {
		// stack
		auto idx = item * 2;
		auto val = *(ssh_w*)(memZX + idx);
		StringZX chars((ssh_b*)&memZX[val], 4);
		auto sp = (idx == *_SP ? L"SP " : L"   ");
		swprintf_s(tmpBuf, 260, dec ? L"%s%05d %05d %s" : L"%s%04X %04X %s", sp, idx, val, chars.buffer());
		txt = tmpBuf;
	}
	auto isStat = (dis->itemState & ODS_SELECTED) != 0;
	HDC hdc = dis->hDC;
	SetBkColor(hdc, GetSysColor(isStat ? COLOR_HIGHLIGHT : COLOR_WINDOW));
	SetTextColor(hdc, GetSysColor(isStat ? COLOR_BTNHIGHLIGHT : COLOR_WINDOWTEXT));
	FillRect(hdc, &dis->rcItem, isStat ? hbrSel : hbrUnSel);
	DrawText(hdc, txt, (int)txt.length(), &dis->rcItem, DT_SINGLELINE | DT_LEFT | DT_EXPANDTABS);
}

INT_PTR zxDebugger::procDlg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch(msg) {
		case WM_COMMAND: {
			int wmId = LOWORD(wParam);
			switch(wmId) {
				case IDM_PREV_BREAKPOINT:
					break;
				case IDM_NEXT_BREAKPOINT:
					break;
				case IDM_BREAKPOINT:
					break;
				case IDM_LIST_BREAKPOINTS:
					break;
				case IDM_STEP_INFO:
					break;
				case IDM_STEP_OVER:
					break;
				case IDM_RUN:
					break;
				case IDM_HEX_DEC:
					updateHexDec(true);
					break;
				case IDM_OVER_PROC:
					break;
				case IDC_BUTTON_TO_PC:
					updateRegisters(_PC, false);
					break;
				case IDC_BUTTON_TO_SP:
					updateStack(*_SP);
					break;
				case IDC_BUTTON_SET_COMMAND:
					break;
				case IDC_LIST_DA: {
					int sel = (int)SendMessage((HWND)lParam, LB_GETCURSEL, 0, 0) - topIndexDA;
					switch(HIWORD(wParam)) {
						case LBN_DBLCLK:
							// определить тип комманды с адресом перехода
							updateRegisters(da->getCmdOperand(sel, true), false);
							break;
						case LBN_SELCHANGE:
							SetWindowText(GetDlgItem(hWnd, IDC_EDIT_CURRENT_COMMAND), da->makeCommand(sel, theApp.getOpt(OPT_DECIMAL)->dval));
							break;
					}
					break;
				}
				case IDC_LIST_STACK:
					switch(HIWORD(wParam)) {
						case LBN_DBLCLK: {
							ssh_w newPC = *(ssh_w*)(memZX + SendMessage((HWND)lParam, LB_GETCURSEL, 0, 0) * 2);
							updateRegisters(newPC, false);
							break;
						}
					}
					break;
			}
			break;
		}
		case WM_CTLCOLORSTATIC: {
			auto id = GetDlgCtrlID((HWND)lParam);
			for(auto& zx : dlgElems) {
				if(zx.idText == id) {
					HDC hdcStatic = (HDC)wParam;
					SetTextColor(hdcStatic, zx.change ? RGB(255, 0, 0) : RGB(0, 0, 0));
					SetBkColor(hdcStatic, GetSysColor(COLOR_BTNFACE));
					return (INT_PTR)::GetStockObject(LTGRAY_BRUSH);
				}
			}
			break;
		}
		case WM_INITDIALOG: init(hWnd); return 1;
		case WM_DRAWITEM: drawItem((DRAWITEMSTRUCT*)lParam, (int)wParam); return 1;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

bool zxDebugger::checkBPS(ssh_w address, bool mem) {
	return false;
}
