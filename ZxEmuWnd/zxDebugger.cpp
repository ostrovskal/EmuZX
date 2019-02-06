#include "stdafx.h"
#include "resource.h"
#include "zxDebugger.h"
#include "zxDisAsm.h"
#include "zxAssembler.h"
#include "CpuZX.h"
#include "DecoderZX.h"
#include "SettingsZX.h"
#include "zxDlgListBps.h"
#include "zxDlgAddBp.h"
#include <math.h>

struct ZX_REG_TMP {
	ssh_w* ptr;
	ssh_cws name;
};

static ZX_REG_TMP regs[] = {
	{(ssh_w*)&regsZX[RC + 8], L"BC'"},{(ssh_w*)&regsZX[RE + 8], L"DE'"},{(ssh_w*)&regsZX[RL + 8], L"HL'"},
	{(ssh_w*)&regsZX[RC], L"BC"},{(ssh_w*)&regsZX[RE], L"DE"},{(ssh_w*)&regsZX[RL], L"HL"},
	{(ssh_w*)&regsZX[RSPL], L"SP"},{(ssh_w*)&regsZX[RPCL], L"PC"},{(ssh_w*)&regsZX[RXL], L"IX"},{(ssh_w*)&regsZX[RYL], L"IY"}
};

BEGIN_MSG_MAP(zxDebugger, zxDialog)
	ON_WM_DRAWITEM()
	ON_WM_CLOSE()
	ON_WM_ERASEBKGND()
	ON_COMMAND(IDM_PREV_BREAKPOINT, onPrevBreakpoint)
	ON_COMMAND(IDM_NEXT_BREAKPOINT, onNextBreakpoint)
	ON_COMMAND(IDM_QUICK_BREAKPOINT, onQuckBreakpoint)
	ON_COMMAND(IDM_LIST_BREAKPOINTS, onListBreakpoint)
	ON_COMMAND(IDM_STEP_INTO, onStepInto)
	ON_COMMAND(IDM_STEP_OVER, onStepOver)
	ON_COMMAND(IDM_PAUSE, onPause)
	ON_COMMAND(IDM_RUN, onRun)
	ON_COMMAND(IDM_PC_UNDO, onPcUndo)
	ON_COMMAND(IDM_PC_REDO, onPcRedo)
	ON_COMMAND(IDM_HEX_DEC, onHexDec)
	ON_COMMAND(IDM_OVER_PROC, onOverProc)
	ON_COMMAND(IDC_BUTTON_TO_PC, onSetPC)
	ON_COMMAND(IDC_BUTTON_TO_SP, onSetSP)
	ON_COMMAND(IDC_BUTTON_SET_DATA, onSetData)
	ON_COMMAND(IDC_BUTTON_SET_COMMAND, onSetCmd)
	ON_CONTROL(LBN_DBLCLK, IDC_LIST_DA, onDblkClkListDA)
	ON_CONTROL(LBN_DBLCLK, IDC_LIST_SP, onDblkClkListSP)
	ON_CONTROL(LBN_VSCROLL, IDC_LIST_DA, onScrollListDA)
	ON_CONTROL(LBN_SELCHANGE, IDC_LIST_DA, onSelChangeListDA)
	ON_CONTROL_RANGE(EN_CHANGE, IDC_EDIT_C, IDC_EDIT_VID, onChangeEditRegs)
END_MSG_MAP()

TBBUTTON tbb[] = {
	{0, IDM_PREV_BREAKPOINT, TBSTATE_ENABLED, BTNS_AUTOSIZE,{0}, 0, (INT_PTR)L"Previous Breakpoint\tCtr+F9"},
	{1, IDM_QUICK_BREAKPOINT, TBSTATE_ENABLED, BTNS_AUTOSIZE, {0}, 0, (INT_PTR)L"Установить/Удалить точку останова\tF9"},
	{2, IDM_NEXT_BREAKPOINT, TBSTATE_ENABLED, BTNS_AUTOSIZE,{0}, 0, (INT_PTR)L"Next Breakpoint\tShift+F9"},
	{3, IDM_LIST_BREAKPOINTS, TBSTATE_ENABLED, BTNS_AUTOSIZE,{0}, 0, (INT_PTR)L"Список точек останова\tF8"},
	{10, -1, TBSTATE_ENABLED, BTNS_SEP,{0}, 0, 0},
	{4, IDM_STEP_INTO, TBSTATE_ENABLED, BTNS_AUTOSIZE,{0}, 0, (INT_PTR)L"Шаг с заходом\tF10"},
	{5, IDM_STEP_OVER, TBSTATE_ENABLED, BTNS_AUTOSIZE,{0}, 0, (INT_PTR)L"Шаг с обходом\tF11"},
	{10, -1, TBSTATE_ENABLED, BTNS_SEP,{0}, 0, 0},
	{6, IDM_PAUSE, TBSTATE_ENABLED, BTNS_AUTOSIZE,{0}, 0, (INT_PTR)L"Приостановить\tF4"},
	{7, IDM_RUN, TBSTATE_ENABLED, BTNS_AUTOSIZE,{0}, 0, (INT_PTR)L"Запустить\tF5"},
	{10, -1, TBSTATE_ENABLED, BTNS_SEP,{0}, 0, 0},
	{8, IDM_PC_UNDO, TBSTATE_ENABLED, BTNS_AUTOSIZE,{0}, 0, (INT_PTR)L"Undo\tCtrl+Z"},
	{9, IDM_PC_REDO, TBSTATE_ENABLED, BTNS_AUTOSIZE,{0}, 0, (INT_PTR)L"Redo\tCtrl+Y"},
	{10, -1, TBSTATE_ENABLED, BTNS_SEP,{0}, 0, 0},
	{10, IDM_HEX_DEC, TBSTATE_ENABLED, BTNS_AUTOSIZE,{0}, 0, (INT_PTR)L"Hex/Dec"},
	{10, -1, TBSTATE_ENABLED, BTNS_SEP,{0}, 0, 0},
	{11, IDM_OVER_PROC, TBSTATE_ENABLED, BTNS_AUTOSIZE,{0}, 0, (INT_PTR)L"Шаг с выходом\tShift+F11"}
};

ZX_DEBUGGER dlgElems[] = {
	{L"BC", IDC_EDIT_BC, IDC_STATIC_BC, nullptr, (ssh_w*)&regsZX[RC], 0xffff},
	{L"DE", IDC_EDIT_DE, IDC_STATIC_DE, nullptr, (ssh_w*)&regsZX[RE], 0xffff},
	{L"HL", IDC_EDIT_HL, IDC_STATIC_HL, nullptr, (ssh_w*)&regsZX[RL], 0xffff},
	{L"AF", IDC_EDIT_AF, IDC_STATIC_AF, nullptr, (ssh_w*)&regsZX[RF], 0xffff},
	{L"BC'", IDC_EDIT_BC_, IDC_STATIC_BC_, nullptr, (ssh_w*)&regsZX[RC_], 0xffff},
	{L"DE'", IDC_EDIT_DE_, IDC_STATIC_DE_, nullptr, (ssh_w*)&regsZX[RE_], 0xffff},
	{L"HL'", IDC_EDIT_HL_, IDC_STATIC_HL_, nullptr, (ssh_w*)&regsZX[RL_], 0xffff},
	{L"AF'", IDC_EDIT_AF_, IDC_STATIC_AF_, nullptr, (ssh_w*)&regsZX[RF_], 0xffff},
	{L"SP", IDC_EDIT_SP, IDC_STATIC_SP, nullptr, (ssh_w*)&regsZX[RSPL], 0xffff},
	{L"IX", IDC_EDIT_IX, IDC_STATIC_IX, nullptr, (ssh_w*)&regsZX[RXL], 0xffff},
	{L"IY ", IDC_EDIT_IY, IDC_STATIC_IY, nullptr, (ssh_w*)&regsZX[RYL], 0xffff},
	{L"PC", IDC_EDIT_PC, IDC_STATIC_PC, nullptr, (ssh_w*)&regsZX[RPCL], 0xffff},
	{L"IFF1", IDC_EDIT_IFF1, IDC_STATIC_IFF1, &regsZX[IFF1], nullptr, 1},
	{L"IFF2", IDC_EDIT_IFF2, IDC_STATIC_IFF2, &regsZX[IFF2], nullptr, 1},
	{L"RAM", IDC_EDIT_RAM, IDC_STATIC_RAM, &regsZX[RAM], nullptr, 7},
	{L"ROM", IDC_EDIT_ROM, IDC_STATIC_ROM, &regsZX[ROM], nullptr, 1},
	{L"IM", IDC_EDIT_IM, IDC_STATIC_IM, &regsZX[IM], nullptr, 0x3},
	{L"VID", IDC_EDIT_VID, IDC_STATIC_VID, &regsZX[VID], nullptr, 0x7},
	{L"SCAN", IDC_EDIT_SCAN, IDC_STATIC_SCAN, &regsZX[SCAN], nullptr, 0xff},
	{L" I ", IDC_EDIT_I, IDC_STATIC_I, &regsZX[RI], nullptr, 0xff},
	{L" R ", IDC_EDIT_R, IDC_STATIC_R, &regsZX[RR], nullptr, 0xff},
	{L" S ", IDC_EDIT_S, IDC_STATIC_S, &regsZX[RF], nullptr, FS, 7},
	{L" Z ", IDC_EDIT_Z, IDC_STATIC_Z, &regsZX[RF], nullptr, FZ, 6},
	{L" 5 ", IDC_EDIT_5, IDC_STATIC_5, &regsZX[RF], nullptr, 32, 5},
	{L" H ", IDC_EDIT_H, IDC_STATIC_H, &regsZX[RF], nullptr, FH, 4},
	{L" 3 ", IDC_EDIT_3, IDC_STATIC_3, &regsZX[RF], nullptr, 8,  3},
	{L" P ", IDC_EDIT_P, IDC_STATIC_P, &regsZX[RF], nullptr, FPV,2},
	{L" N ", IDC_EDIT_N, IDC_STATIC_N, &regsZX[RF], nullptr, FN, 1},
	{L" C ", IDC_EDIT_C, IDC_STATIC_C, &regsZX[RF], nullptr, FC, 0}
};

BOOL zxDebugger::onEraseBkgnd(HDC hdc) {
	return FALSE;
}

void zxDebugger::onPrevBreakpoint() {
	if(curIndexBP > 0) {
		if(bps[curIndexBP - 1].flags & FBP_ADDR) {
			curIndexBP--;
			updateRegisters(bps[curIndexBP].address1, U_PC | U_SEL | U_TOP);
		}
	}
	updatePrevNextBP();
}

void zxDebugger::onNextBreakpoint() {
	if(curIndexBP < (COUNT_BP - 1)) {
		if(bps[curIndexBP + 1].flags & FBP_ADDR) {
			curIndexBP++;
			updateRegisters(bps[curIndexBP].address1, U_PC | U_SEL | U_TOP);
		}
	}
	updatePrevNextBP();
}

void zxDebugger::onQuckBreakpoint() {
	int sel = (int)SendMessage(hWndDA, LB_GETCURSEL, 0, 0) - _pc;
	auto addr = da->getCmdAddress(sel);
	if(addr != -1) {
		quickBP(addr);
		InvalidateRect(hWndDA, nullptr, false);
	}
}

void zxDebugger::onListBreakpoint() {
	zxDlgListBps dlg;
	dlg.create(IDD_DIALOG_LIST_BPS, IDA_ACCEL_LIST_BPS, this, true);
	InvalidateRect(hWndDA, nullptr, false);
}

void zxDebugger::onStepInto() {
	int addr1, addr2;

	int sel = (int)SendMessage(hWndDA, LB_GETCURSEL, 0, 0) - _pc;
	da->getCmdOperand(sel, true, true, true, &addr1, &addr2);
	if(addr1 == -1) addr1 = addr2 = da->move((*_PC), 1);
	while(true) {
		theApp.zilog->execute(true);
		if((*_PC) == addr1 || (*_PC) == addr2) break;
	}
	updateRegisters((*_PC), U_REGS | U_PC | U_SP | U_SEL);
}

void zxDebugger::onStepOver() {
	int addr1, addr2;
	
	int sel = (int)SendMessage(hWndDA, LB_GETCURSEL, 0, 0) - _pc;
	da->getCmdOperand(sel, true, false, true, &addr1, &addr2);
	if(addr1 == -1) addr1 = addr2 = da->move((*_PC), 1);
	while(true) {
		theApp.zilog->execute(true);
		if((*_PC) == addr1 || (*_PC) == addr2) break;
	}
	updateRegisters((*_PC), U_REGS | U_PC | U_SP | U_SEL);
}

void zxDebugger::onPause() {
	setProgrammPause(true, false);
}

void zxDebugger::onRun() {
	theApp.zilog->execute(true);
	setProgrammPause(false, false);
}

void zxDebugger::onPcUndo() {
	if(curStoryPC > 0) {
		curStoryPC--;
		updateRegisters(storyPC[curStoryPC], U_PC | U_SEL | U_TOP);
	}
}

void zxDebugger::onPcRedo() {
	if(curStoryPC < limitStoryPC) {
		curStoryPC++;
		updateRegisters(storyPC[curStoryPC], U_PC | U_SEL | U_TOP);
	}
}

void zxDebugger::onHexDec() {
	updateHexDec(true);
}

void zxDebugger::onOverProc() {
	auto exit_pc = *_PC_EXIT_CALL;
	while((*_PC) != exit_pc) theApp.zilog->execute(true);
	updateRegisters((*_PC), U_REGS | U_PC | U_SP | U_SEL);
}

void zxDebugger::onSetPC() {
	updateRegisters((*_PC), U_PC | U_SEL | U_TOP);
}

void zxDebugger::onSetSP() {
	updateStack(*_SP);
}

void zxDebugger::onSetData() {
	HWND h = GetDlgItem(hWnd, IDC_EDIT_ADDRESS_DATA);
	GetWindowText(h, tmpBuf, 260);
	auto addr = comparePredefinedNames(tmpBuf);
	if(addr != -1) _dt = addr; else {
		_dt = *(ssh_w*)asm_ssh_wton(tmpBuf, (ssh_u)Radix::_dec);
	}
	SendMessage(hWndDT, LB_SETTOPINDEX, _dt / 16, 0);
	InvalidateRect(hWndDT, nullptr, true);
}

void zxDebugger::onSetCmd() {
	HWND h = GetDlgItem(hWnd, IDC_EDIT_CURRENT_COMMAND);
	GetWindowText(h, tmpBuf, 260);
	int posErr = 0;
	int len = assm->parseCommand(tmpBuf, FASM_LABEL, &posErr);
	if(len) {
		// вставить комманду
		int sel = (int)SendMessage(hWndDA, LB_GETCURSEL, 0, 0) - _pc;
		memcpy(&memZX[da->getCmdAddress(sel)], assm->code, len);
		da->decode(_pc, countVisibleItems);
		InvalidateRect(hWndDA, nullptr, false);
	} else {
		SendMessage(h, EM_SETSEL, posErr, posErr + 100);
	}
}

void zxDebugger::onDblkClkListDA() {
	// определить тип комманды с адресом перехода
	int addr1, addr2;

	int sel = (int)SendMessage(hWndDA, LB_GETCURSEL, 0, 0) - _pc;
	da->getCmdOperand(sel, true, true, false, &addr1, &addr2);
	if(addr1 != -1) updateRegisters(addr1, U_PC | U_STORY | U_SEL | U_TOP);
	else {
		da->getCmdOperand(sel, false, false, false, &addr1, &addr2);
		if(addr1 != -1) {
			int dec = theApp.getOpt(OPT_DECIMAL)->dval;
			fromNum(addr1, radix[dec + 22]);
			SetWindowText(GetDlgItem(hWnd, IDC_EDIT_ADDRESS_DATA), tmpBuf);
			_dt = addr1; SendMessage(hWndDT, LB_SETTOPINDEX, _dt / 16, 0);
			InvalidateRect(hWndDT, nullptr, false);
		}
	}
}

void zxDebugger::onDblkClkListSP() {
	ssh_w newPC = *(ssh_w*)(memZX + SendMessage(hWndSP, LB_GETCURSEL, 0, 0) * 2);
	updateRegisters(newPC, U_PC | U_STORY | U_SEL | U_TOP);
}

void zxDebugger::onScrollListDA() {
	int delta = (int)SendMessage(hWndDA, LB_GETTOPINDEX, 0, 0) - _pc;
	_pc = da->move(_pc, delta);
	_lastPC = da->decode(_pc, countVisibleItems);
	SendMessage(hWndDA, LB_SETTOPINDEX, _pc, 0);
}

void zxDebugger::onSelChangeListDA() {
	int sel = (int)SendMessage(hWndDA, LB_GETCURSEL, 0, 0) - _pc;
	SetWindowText(GetDlgItem(hWnd, IDC_EDIT_CURRENT_COMMAND), da->makeCommand(sel, theApp.getOpt(OPT_DECIMAL)->dval));
}

void zxDebugger::onChangeEditRegs() {
	bool isFlag = wmId < IDC_EDIT_BC;
	for(auto& zx : dlgElems) {
		if(zx.idMain == wmId) {
			GetWindowText(zx.hWndMain, tmpBuf, 256);
			ssh_w v = (ssh_w)*(ssh_u*)(asm_ssh_wton(tmpBuf, (ssh_u)Radix::_dec));
			if(zx.regb) {
				if(isFlag) {
					*zx.regb &= ~(1 << zx.shift);
					*zx.regb |= (v ? 1 : 0) << zx.shift;
				} else *zx.regb = (ssh_b)(v & zx.msk);
			} else if(zx.regw) {
				v &= zx.msk;
				*zx.regw = v;
			}
		}
	}
}

void zxDebugger::onClose() {
	modifyTSTATE(ZX_EXEC, ZX_DEBUG);
	theApp.changeExecute(false);
	theApp.changeWndDebugger(true);
}

void zxDebugger::postCreate() {
	hWndDA = zxDA.create(L"zxListBox", nullptr, WS_VISIBLE | WS_CHILD | WS_VSCROLL | WS_BORDER, 5, 33, 379, 515, this, IDC_LIST_DA, 0);
	hWndSP = zxSP.create(L"zxListBox", nullptr, WS_VISIBLE | WS_CHILD | WS_VSCROLL | WS_BORDER, 390, 338, 171, 210, this, IDC_LIST_SP, 0);
	hWndDT = zxDT.create(L"zxListBox", nullptr, WS_VISIBLE | WS_CHILD | WS_VSCROLL | WS_BORDER, 5, 575, 558, 131, this, IDC_LIST_DT, 0);

	for(auto& zx : dlgElems) {
		zx.hWndMain = GetDlgItem(hWnd, zx.idMain);
		zx.hWndText = GetDlgItem(hWnd, zx.idText);
		zx.val = (((zx.regb == nullptr) ? *zx.regw : (ssh_w)*zx.regb) & zx.msk) >> zx.shift;
	}
	SendMessage(hWndDA, WM_SETFONT, WPARAM(hFont), TRUE);
	SendMessage(hWndDA, LB_SETITEMHEIGHT, 0, 16);
	SendMessage(hWndDA, LB_SETCOUNT, 65534, 0);

	countVisibleItems = (int)SendMessage(hWndDA, LB_GETCOUNT, 1, 0);

	SendMessage(hWndSP, WM_SETFONT, WPARAM(hFont), TRUE);
	SendMessage(hWndSP, LB_SETITEMHEIGHT, 0, 16);
	SendMessage(hWndSP, LB_SETCOUNT, 32768, 0);

	SendMessage(hWndDT, WM_SETFONT, WPARAM(hFont), TRUE);
	SendMessage(hWndDT, LB_SETITEMHEIGHT, 0, 16);
	SendMessage(hWndDT, LB_SETCOUNT, 4097, 0);
	_sp = -1;
}

void zxDebugger::onDrawItem(int id, LPDRAWITEMSTRUCT dis) {
	StringZX txt;

	auto dec = theApp.getOpt(OPT_DECIMAL)->dval;
	auto item = dis->itemID;
	if(id == IDC_LIST_DA) {
		auto top = SendMessage(hWndDA, LB_GETTOPINDEX, 0, 0);
		txt = da->makeCommand((int)(item - top), DA_FCODE | DA_FADDR | DA_FPADDR | (int)dec);
	} else if(id == IDC_LIST_SP) {
		// stack
		auto idx = item * 2;
		auto val = *(ssh_w*)(memZX + idx);
		StringZX chars;
		for(int i = 0; i < 4; i++) {
			ssh_w v = (ssh_w)memZX[val + i];
			if(v < 32) v = 32;
			chars += (ssh_ws)v;
		}
		auto sp = ((idx == (*_SP)) ? L"SP " : L"   ");
		txt = StringZX::fmt(dec ? L"%s%05d %05d %s" : L"%s%04X %04X %s", sp, idx, val, chars.buffer());
	} else if(id == IDC_LIST_DT) {
		// адресс 16 значений текстовое представление
		auto idx = (_dt % 16) + (item * 16);
		if(idx > 65535) return;
		StringZX vals(fromNum(idx, radix[dec + 22]));
		vals += L"  ";
		StringZX chars;
		for(int i = 0; i < 16; i++) {
			if(idx > 65535) break;
			ssh_w v = (ssh_w)memZX[idx++];
			fromNum(v, radix[dec + 2]);
			vals += tmpBuf;
			if(v < 32) v = 32;
			chars += (ssh_ws)v;
		}
		txt = StringZX::fmt(L"%s %s", vals.buffer(), chars.buffer());
	}
	auto isStat = (dis->itemState & ODS_SELECTED) != 0;
	HDC hdc = dis->hDC;
	SetBkColor(hdc, GetSysColor(isStat ? COLOR_HIGHLIGHT : COLOR_WINDOW));
	SetTextColor(hdc, GetSysColor(isStat ? COLOR_BTNHIGHLIGHT : COLOR_WINDOWTEXT));
	DrawText(hdc, txt, (int)txt.length(), &dis->rcItem, DT_SINGLELINE | DT_LEFT | DT_EXPANDTABS);
}

zxDebugger::~zxDebugger() {
	SAFE_DELETE(da);
	SAFE_DELETE(assm);
	DeleteObject(hFont);
	DeleteObject(hbrSel);
	DeleteObject(hbrUnsel);
}

bool zxDebugger::preCreate() {
	hFont = CreateFont(-12, 0, 0, 0, FW_THIN, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_TT_PRECIS,
					   CLIP_DEFAULT_PRECIS, PROOF_QUALITY, FF_ROMAN, L"Courier New");

	hbrSel = CreateSolidBrush(GetSysColor(COLOR_HIGHLIGHT));
	hbrUnsel = CreateSolidBrush(GetSysColor(COLOR_WINDOW));

	da = new zxDisAsm;
	assm = new zxAssembler;
	// прочитать точки останова из установок
	int count = 0;
	for(int i = OPT_BPS0; i <= OPT_BPS9; i++) {
		auto opt = theApp.getOpt(i);
		if(opt->sval.is_empty()) continue;
		auto vals = opt->sval.split(L",", count);
		if(count == 5) {
			auto bp = &bps[i - OPT_BPS0];
			bp->address1= _wtoi(vals[0]);
			bp->address2= _wtoi(vals[1]);
			bp->value	= _wtoi(vals[2]);
			bp->mask	= _wtoi(vals[3]);
			bp->flags	= _wtoi(vals[4]);
		}
	}
	return true;
}

void zxDebugger::show(bool visible) {
	if(visible) {
		if(!hWndToolbar) makeToolbar(IDB_TOOLBAR_DEBUGGER, tbb, 15, 17, 16, 16, IDT_TOOLBAR_DEBUGGER);
		ShowWindow(hWnd, true);
		UpdateWindow(hWnd);
		updateHexDec(false);
		setProgrammPause(true, false);
		updateUndoRedo(false);
	} else {
		if(IsWindowVisible(hWnd)) {
			ShowWindow(hWnd, false);
			modifyTSTATE(ZX_EXEC, ZX_DEBUG);
			theApp.changeExecute(false);
			// записать точки останова в установки
			saveBPS();
		}
	}
}

void zxDebugger::updateRegisters(int newPC, int flags) {
	bool decimal = theApp.getOpt(OPT_DECIMAL)->dval;
	HWND h;
	RECT rect;
	if(flags & U_REGS) {
		for(auto& zx : dlgElems) {
			ssh_w val = (((zx.regb == nullptr) ? *zx.regw : (ssh_w)(*zx.regb)) & zx.msk) >> zx.shift;
			zx.change = zx.val != val;
			h = zx.hWndText;
			auto hdc = GetDC(h);
			SetTextColor(hdc, zx.change ? RGB(0, 255, 0) : RGB(0, 0, 0));
			//SetBkMode(hdc, TRANSPARENT);
			SelectObject(hdc, hFont);
			GetClientRect(h, &rect);
			DrawText(hdc, zx.text, -1, &rect, DT_SINGLELINE | DT_CENTER);
			ReleaseDC(h, hdc);
			zx.val = val;
			auto fmt = (zx.msk < 255 ? L"%d" : (zx.regb ? radix[decimal] : radix[decimal + 22]));
			SetWindowText(zx.hWndMain, fromNum(val, fmt));
		}
		InvalidateRect(hWnd, nullptr, false);
	}
	if((flags & U_SP) && _SP && _sp != *_SP) updateStack(*_SP);
	
	if(flags & U_PC) {
		if(_pc != newPC) {
			if(newPC < _pc || newPC >= _lastPC) {
				_lastPC = da->decode(newPC, countVisibleItems);
				_pc = newPC;
			}
			updateUndoRedo(flags & U_STORY);
			SendMessage(hWndDA, LB_SETTOPINDEX, _pc, 0);
		}
		if(flags & U_SEL) SendMessage(hWndDA, LB_SETCURSEL, _pc + da->indexFromAddress(newPC), 0);
	}
}

void zxDebugger::updateStack(int sp) {
	sp -= 10;
	if(sp < 0) sp = 0;
	else if((sp + 20) > 65535) sp = 655508;
	_sp = sp / 2;
	SendMessage(hWndSP, LB_SETTOPINDEX, _sp, 0);
	SendMessage(hWndSP, LB_SETCURSEL, (*_SP) / 2, 0);
	InvalidateRect(hWndSP, nullptr, true);
}

void zxDebugger::updatePrevNextBP() {
	if(hWndToolbar) {
		bool is = curIndexBP > 0 && (bps[curIndexBP - 1].flags & FBP_ADDR);
		SendMessage(hWndToolbar, TB_SETSTATE, IDM_PREV_BREAKPOINT, (is << 2));
		is = curIndexBP < (COUNT_BP - 1) && (bps[curIndexBP + 1].flags & FBP_ADDR);
		SendMessage(hWndToolbar, TB_SETSTATE, IDM_NEXT_BREAKPOINT, is << 2);
	}
}

void zxDebugger::updateUndoRedo(bool set) {
	if(set) {
		if(curStoryPC > (COUNT_STORY_PC - 1)) {
			memcpy(&storyPC[0], &storyPC[1], (COUNT_STORY_PC - 1) * 2);
			curStoryPC = COUNT_STORY_PC - 1;
		}
		storyPC[++curStoryPC] = _pc;
		limitStoryPC = curStoryPC;
	}
	if(hWndToolbar) {
		SendMessage(hWndToolbar, TB_SETSTATE, IDM_PC_UNDO, ((curStoryPC > 0) << 2));
		SendMessage(hWndToolbar, TB_SETSTATE, IDM_PC_REDO, ((curStoryPC < limitStoryPC) << 2));
	}
}

void zxDebugger::updateHexDec(bool change) {
	auto opt = theApp.getOpt(OPT_DECIMAL);
	if(change) {
		opt->dval = !opt->dval;
		updateRegisters(_pc, U_PC | U_SP | U_REGS);
		InvalidateRect(hWndSP, nullptr, false);
		InvalidateRect(hWndDA, nullptr, false);
		InvalidateRect(hWndDT, nullptr, true);
	}
	if(hWndToolbar) SendMessage(hWndToolbar, TB_SETSTATE, IDM_HEX_DEC, TBSTATE_ENABLED | opt->dval);
}

int zxDebugger::comparePredefinedNames(ssh_cws buf) {
	for(auto& r : regs) {
		if(wcscmp(r.name, buf) == 0) return *r.ptr;
	}
	return -1;
}

bool zxDebugger::check(ssh_w address, ZX_BREAK_POINT& breakpt, int flags) {
	for(auto& bp : bps) {
		if(bp.flags & FBP_ADDR) {
			if(address >= bp.address1 && address <= bp.address2 && (bp.flags & flags)) {
				breakpt = bp;
				return true;
			}
		}
	}
	return false;
}

void zxDebugger::setProgrammPause(bool pause, bool activate) {
	if(hWndToolbar) {
		int state1 = pause << 2;
		int state2 = (!pause) << 2;

		SendMessage(hWndToolbar, TB_SETSTATE, IDM_PAUSE, state2);
		SendMessage(hWndToolbar, TB_SETSTATE, IDM_STEP_OVER, state1);
		SendMessage(hWndToolbar, TB_SETSTATE, IDM_STEP_INTO, state1);
		SendMessage(hWndToolbar, TB_SETSTATE, IDM_RUN, state1);
		SendMessage(hWndToolbar, TB_SETSTATE, IDM_OVER_PROC, state1);

		updateUndoRedo(false);
		updatePrevNextBP();

		modifyTSTATE(pause ? ZX_DEBUG : ZX_EXEC, pause ? ZX_EXEC : 0);
		if(pause) updateRegisters((*_PC), U_PC | U_REGS | U_STORY | U_SEL | U_SP | U_TOP);
		theApp.changeExecute(false);

		if(activate) SetActiveWindow(hWnd);
	}
}

 bool zxDebugger::checkBPS(ssh_w address, bool mem) {
	static ZX_BREAK_POINT bp;
	bool res = false;
	int flags = (mem ? FBP_MEM : FBP_PC);
	if(check(address, bp, flags)) {
		res = true;
		if(flags == FBP_MEM) {
			if(bp.flags & FBP_VAL) {
				ssh_b v1 = memZX[address];
				if(bp.flags & FBP_MASK) v1 &= bp.mask;
				ssh_b v2 = (ssh_b)bp.value;
				switch((bp.flags & FBP_COND)) {
					case FBP_EQ: res = v1 == v2; break;
					case FBP_NEQ: res = v1 != v2; break;
					case FBP_GT: res = v1 > v2; break;
					case FBP_LS: res = v1 < v2; break;
					case FBP_QGT: res = v1 >= v2; break;
					case FBP_QLS: res = v1 <= v2; break;
				}
			}
		}
		if(res) setProgrammPause(true, true);
	}
	return res;
}

void zxDebugger::quickBP(int adr) {
	for(int i = 0; i < COUNT_BP; i++) {
		auto bp = &bps[i];
		if(!(bp->flags & FBP_ADDR)) {
			bp->address1= adr;
			bp->address2= adr;
			bp->value	= 0;
			bp->mask	= 0;
			bp->flags	= FBP_ADDR | FBP_PC;
			break;
		} else if(adr >= bp->address1 && adr <= bp->address2 && (bp->flags & FBP_PC)) {
			removeBP(i);
			break;
		}
	}
	updatePrevNextBP();
}

void zxDebugger::saveBPS() {
	// записать точки останова в установки
	for(int i = OPT_BPS0; i <= OPT_BPS9; i++) {
		auto bp = &bps[i - OPT_BPS0];
		theApp.getOpt(i)->sval = StringZX::fmt(L"%d,%d,%d,%d,%d", bp->address1, bp->address2, bp->value, bp->mask, bp->flags);
	}
}

void zxDebugger::removeBP(int index) {
	if(curIndexBP == index) curIndexBP--;
	memcpy(&bps[index], &bps[index + 1], (&bps[(COUNT_BP - 1)] - &bps[index]) * sizeof(ZX_BREAK_POINT));
	memset(&bps[(COUNT_BP - 1)], 0, sizeof(ZX_BREAK_POINT));
}
