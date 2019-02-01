#include "stdafx.h"
#include "resource.h"
#include "zxDebugger.h"
#include "zxDisAsm.h"
#include "CpuZX.h"
#include "DecoderZX.h"
#include "SettingsZX.h"
#include "zxDlgListBps.h"
#include "zxDlgAddBp.h"
#include <math.h>

BEGIN_MSG_MAP(zxDebugger, zxDialog)
END_MSG_MAP()

TBBUTTON tbb[] = {
	{0, IDM_PREV_BREAKPOINT, TBSTATE_ENABLED, BTNS_AUTOSIZE,{0}, 0, (INT_PTR)L"Previous Breakpoint\tCtr+F9"},
	{1, IDM_BREAKPOINT, TBSTATE_ENABLED, BTNS_AUTOSIZE,{0}, 0, (INT_PTR)L"Установить/Удалить точку останова\tF9"},
	{2, IDM_NEXT_BREAKPOINT, TBSTATE_ENABLED, BTNS_AUTOSIZE,{0}, 0, (INT_PTR)L"Next Breakpoint\tShift+F9"},
	{3, IDM_LIST_BREAKPOINTS, TBSTATE_ENABLED, BTNS_AUTOSIZE,{0}, 0, (INT_PTR)L"Список точек останова\tF8"},
	{10, -1, TBSTATE_ENABLED, BTNS_SEP,{0}, 0, 0},
	{4, IDM_STEP_INFO, TBSTATE_ENABLED, BTNS_AUTOSIZE,{0}, 0, (INT_PTR)L"Шаг с заходом\tF10"},
	{5, IDM_STEP_OVER, TBSTATE_ENABLED, BTNS_AUTOSIZE,{0}, 0, (INT_PTR)L"Шаг с обходом\tF11"},
	{10, -1, TBSTATE_ENABLED, BTNS_SEP,{0}, 0, 0},
	{6, IDM_PAUSE, TBSTATE_ENABLED, BTNS_AUTOSIZE,{0}, 0, (INT_PTR)L"Приостановить\tF4"},
	{7, IDM_RUN, TBSTATE_ENABLED, BTNS_AUTOSIZE,{0}, 0, (INT_PTR)L"Запустить\tF5"},
	{10, -1, TBSTATE_ENABLED, BTNS_SEP,{0}, 0, 0},
	{8, IDM_UNDO, TBSTATE_ENABLED, BTNS_AUTOSIZE,{0}, 0, (INT_PTR)L"Undo\tCtrl+Z"},
	{9, IDM_REDO, TBSTATE_ENABLED, BTNS_AUTOSIZE,{0}, 0, (INT_PTR)L"Redo\tCtrl+Y"},
	{10, -1, TBSTATE_ENABLED, BTNS_SEP,{0}, 0, 0},
	{10, IDM_HEX_DEC, TBSTATE_ENABLED, BTNS_AUTOSIZE,{0}, 0, (INT_PTR)L"Hex/Dec"},
	{10, -1, TBSTATE_ENABLED, BTNS_SEP,{0}, 0, 0},
	{11, IDM_OVER_PROC, TBSTATE_ENABLED, BTNS_AUTOSIZE,{0}, 0, (INT_PTR)L"Шаг с выходом\tShift+F11"}
};

ZX_DEBUGGER dlgElems[] = {
	{L"BC", IDC_EDIT_BC, IDC_STATIC_BC, nullptr, (ssh_w*)&regsZX[RC], 0xffff},
	{L"DE", IDC_EDIT_DE, IDC_STATIC_DE, nullptr, (ssh_w*)&regsZX[RE], 0xffff},
	{L"HL", IDC_EDIT_HL, IDC_STATIC_HL, nullptr, (ssh_w*)&regsZX[RL], 0xffff},
	{L"AF", IDC_EDIT_AF, IDC_STATIC_AF, nullptr, (ssh_w*)&regsZX[RA], 0xffff},
	{L"BC'", IDC_EDIT_BC_, IDC_STATIC_BC_, nullptr, (ssh_w*)&regsZX[RC_], 0xffff},
	{L"DE'", IDC_EDIT_DE_, IDC_STATIC_DE_, nullptr, (ssh_w*)&regsZX[RE_], 0xffff},
	{L"HL'", IDC_EDIT_HL_, IDC_STATIC_HL_, nullptr, (ssh_w*)&regsZX[RL_], 0xffff},
	{L"AF'", IDC_EDIT_AF_, IDC_STATIC_AF_, nullptr, (ssh_w*)&regsZX[RA_], 0xffff},
	{L"SP", IDC_EDIT_SP, IDC_STATIC_SP, nullptr, (ssh_w*)&regsZX[RSPL], 0xffff},
	{L"IX", IDC_EDIT_IX, IDC_STATIC_IX, nullptr, (ssh_w*)&regsZX[RIXL], 0xffff},
	{L"IY", IDC_EDIT_IY, IDC_STATIC_IY, nullptr, (ssh_w*)&regsZX[RIYL], 0xffff},
	{L"PC", IDC_EDIT_PC, IDC_STATIC_PC, nullptr, &_PC, 0xffff},
	{L"IFF1", IDC_EDIT_IFF1, IDC_STATIC_IFF1, &regsZX[RIFF1], nullptr, 1},
	{L"IFF2", IDC_EDIT_IFF2, IDC_STATIC_IFF2, &regsZX[RIFF2], nullptr, 1},
	{L"IM", IDC_EDIT_IM, IDC_STATIC_IM, &regsZX[RIM], nullptr, 0x3},
	{L" I ", IDC_EDIT_I, IDC_STATIC_I, &regsZX[RI], nullptr, 0xff},
	{L" R ", IDC_EDIT_R, IDC_STATIC_R, &regsZX[RR], nullptr, 0xff},
	{L" S ", IDC_EDIT_S, IDC_STATIC_S, &regsZX[RF], nullptr, FS, 7},
	{L" Z ", IDC_EDIT_Z, IDC_STATIC_Z, &regsZX[RF], nullptr, FZ, 6},
	{L" 5 ", IDC_EDIT_5, IDC_STATIC_5, &regsZX[RF], nullptr, F5, 5},
	{L" H ", IDC_EDIT_H, IDC_STATIC_H, &regsZX[RF], nullptr, FH, 4},
	{L" 3 ", IDC_EDIT_3, IDC_STATIC_3, &regsZX[RF], nullptr, F3, 3},
	{L" P ", IDC_EDIT_P, IDC_STATIC_P, &regsZX[RF], nullptr, FPV,2},
	{L" N ", IDC_EDIT_N, IDC_STATIC_N, &regsZX[RF], nullptr, FN, 1},
	{L" C ", IDC_EDIT_C, IDC_STATIC_C, &regsZX[RF], nullptr, FC, 0},
	{L"TSTATE", IDC_EDIT_TSTATE1, IDC_STATIC_TSTATE, nullptr, (ssh_w*)&_TSTATE, 0xffff}
};

zxDebugger::~zxDebugger() {
	SAFE_DELETE(da);
	DeleteObject(hFont);
	DeleteObject(hbrSel);
	DeleteObject(hbrUnSel);
}

bool zxDebugger::preCreate() {
	hFont = CreateFont(-12, 0, 0, 0, FW_THIN, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_TT_PRECIS,
					   CLIP_DEFAULT_PRECIS, PROOF_QUALITY, FF_ROMAN, L"MS Shell Dlg");

	hbrSel = CreateSolidBrush(GetSysColor(COLOR_HIGHLIGHT));
	hbrUnSel = CreateSolidBrush(GetSysColor(COLOR_WINDOW));

	da = new zxDisAsm;
	// прочитать точки останова из установок
	int count = 0;
	for(int i = OPT_BPS0; i <= OPT_BPS9; i++) {
		auto opt = theApp.getOpt(i);
		if(opt->sval.is_empty()) continue;
		auto vals = opt->sval.split(L",", count);
		if(count == 4) {
			auto bp = &bps[i - OPT_BPS0];
			bp->address1 = _wtoi(vals[0]);
			bp->address2 = _wtoi(vals[1]);
			bp->value = _wtoi(vals[2]);
			bp->flags = _wtoi(vals[3]);
		}
	}
	return true;
}

void zxDebugger::show(bool visible) {
	if(visible) {
		if(!toolbar) toolbar = new zxToolbar(this, IDB_TOOLBAR_DEBUGGER, tbb, 15, 17, 16, 16, 2000);
		showWindow(true);
		updateWindow();
		updateHexDec(false);
		setProgrammPause(true, false);
		updateUndoRedo(false);
	} else {
		if(isWindowVisible()) {
			showWindow(false);
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
			auto fmt = (zx.msk < 255 ? L"%d" : (zx.regb ? radix[decimal] : radix[decimal + 10]));
			SetWindowText(zx.hWndMain, fromNum(val, fmt));
		}
		InvalidateRect(hWnd, nullptr, false);
	}
	if((flags & U_SP) && _SP && _sp != *_SP) updateStack(*_SP);
	
	if(flags & U_PC) {
		if(_pc != newPC) {
			if(newPC < _pc || newPC >= _lastPC) {
				_lastPC = da->decode(newPC, countVisibleItems + 1);
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
	if(toolbar) {
		bool is = curIndexBP > 0 && (bps[curIndexBP - 1].address1 != -1);
		SendMessage(toolbar->getHWND(), TB_SETSTATE, IDM_PREV_BREAKPOINT, (is << 2));
		is = curIndexBP < (COUNT_BP - 1) && (bps[curIndexBP + 1].address1 != -1);
		SendMessage(toolbar->getHWND(), TB_SETSTATE, IDM_NEXT_BREAKPOINT, is << 2);
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
	if(toolbar) {
		SendMessage(toolbar->getHWND(), TB_SETSTATE, IDM_UNDO, ((curStoryPC > 0) << 2));
		SendMessage(toolbar->getHWND(), TB_SETSTATE, IDM_REDO, ((curStoryPC < limitStoryPC) << 2));
	}
}

void zxDebugger::updateHexDec(bool change) {
	auto opt = theApp.getOpt(OPT_DECIMAL);
	if(change) {
		opt->dval = !opt->dval;
		updateRegisters(_pc, U_PC | U_SP | U_REGS);
		InvalidateRect(hWndSP, nullptr, false);
		InvalidateRect(hWndDA, nullptr, false);
	}
	if(toolbar) SendMessage(toolbar->getHWND(), TB_SETSTATE, IDM_HEX_DEC, TBSTATE_ENABLED | opt->dval);
}

bool zxDebugger::onSize(WPARAM type, int nWidth, int nHeight) {
	return false;
}

bool zxDebugger::onClose() {
	modifyTSTATE(ZX_EXEC, ZX_DEBUG);
	theApp.changeExecute(false);
	theApp.changeWndDebugger(true);
	return false;
}

void zxDebugger::onInitDialog(HWND hWnd, LPARAM lParam) {
	hWndDA = zxDA.create(L"zxListBox", nullptr, WS_VISIBLE | WS_CHILD | WS_VSCROLL | WS_BORDER, 0, 29, 384, 515, this, IDC_LIST_DA, 0);
	hWndSP = zxSP.create(L"zxListBox", nullptr, WS_VISIBLE | WS_CHILD | WS_VSCROLL | WS_BORDER, 390, 327, 173, 218, this, IDC_LIST_STACK, 0);

	for(auto& zx : dlgElems) {
		zx.hWndMain = GetDlgItem(hWnd, zx.idMain);
		zx.hWndText = GetDlgItem(hWnd, zx.idText);
		zx.val = (((zx.regb == nullptr) ? *zx.regw : (ssh_w)*zx.regb) & zx.msk) >> zx.shift;
	}
	SendMessage(hWndDA, WM_SETFONT, WPARAM(hFont), TRUE);
	SendMessage(hWndDA, LB_SETITEMHEIGHT, 0, 16);
	SendMessage(hWndDA, LB_SETCOUNT, 65536, 0);
	SendMessage(hWndDA, LB_SETTOPINDEX, _PC, 0);
	
	countVisibleItems = (int)SendMessage(hWndDA, LB_GETCOUNT, 1, 0);

	SendMessage(hWndSP, WM_SETFONT, WPARAM(hFont), TRUE);
	SendMessage(hWndSP, LB_SETITEMHEIGHT, 0, 16);
	SendMessage(hWndSP, LB_SETCOUNT, 32768, 0);
	_sp = -1;
}

bool zxDebugger::onDrawItem(UINT id, LPDRAWITEMSTRUCT dis) {
	StringZX txt;

	auto dec = theApp.getOpt(OPT_DECIMAL)->dval;
	auto item = dis->itemID;
	if(id == IDC_LIST_DA) {
		auto top = SendMessage(hWndDA, LB_GETTOPINDEX, 0, 0);
		txt = da->makeCommand((int)(item - top), DA_FCODE | DA_FADDR | DA_FPADDR | (int)dec);
	} else {
		// stack
		auto idx = item * 2;
		auto val = *(ssh_w*)(memZX + idx);
		StringZX chars((ssh_b*)&memZX[val], 4);
		auto sp = (idx == *_SP ? L"SP " : L"   ");
		txt = StringZX::fmt(dec ? L"%s%05d %05d %s" : L"%s%04X %04X %s", sp, idx, val, chars.buffer());
	}
	auto isStat = (dis->itemState & ODS_SELECTED) != 0;
	HDC hdc = dis->hDC;
	SetBkColor(hdc, GetSysColor(isStat ? COLOR_HIGHLIGHT : COLOR_WINDOW));
	SetTextColor(hdc, GetSysColor(isStat ? COLOR_BTNHIGHLIGHT : COLOR_WINDOWTEXT));
	DrawText(hdc, txt, (int)txt.length(), &dis->rcItem, DT_SINGLELINE | DT_LEFT | DT_EXPANDTABS);
	return true;
}

bool zxDebugger::onCommand(int wmId, int param, LPARAM lParam) {
	int sel = (int)SendMessage(hWndDA, LB_GETCURSEL, 0, 0) - _pc;
	bool flag = false;
	switch(wmId) {
		case IDM_UNDO:
			if(curStoryPC > 0) {
				curStoryPC--;
				updateRegisters(storyPC[curStoryPC], U_PC | U_SEL | U_TOP);
			}
			break;
		case IDM_REDO:
			if(curStoryPC < limitStoryPC) {
				curStoryPC++;
				updateRegisters(storyPC[curStoryPC], U_PC | U_SEL | U_TOP);
			}
			break;
		case IDM_PREV_BREAKPOINT:
			if(curIndexBP > 0) {
				if(bps[curIndexBP - 1].address1 != -1) {
					curIndexBP--;
					updateRegisters(bps[curIndexBP].address1, U_PC | U_SEL | U_TOP);
				}
			}
			updatePrevNextBP();
			break;
		case IDM_NEXT_BREAKPOINT:
			if(curIndexBP < (COUNT_BP - 1)) {
				if(bps[curIndexBP + 1].address1 != -1) {
					curIndexBP++;
					updateRegisters(bps[curIndexBP].address1, U_PC | U_SEL | U_TOP);
				}
			}
			updatePrevNextBP();
			break;
		case IDM_BREAKPOINT: {
			auto addr = da->getCmdAddress(sel);
			if(addr != -1) {
				quickBP(addr);
				InvalidateRect(hWndDA, nullptr, false);
			}
			break;
		}
		case IDM_LIST_BREAKPOINTS: {
			zxDlgListBps dlg;
			dlg.create(IDD_DIALOG_LIST_BPS, this, true);
			InvalidateRect(hWndDA, nullptr, false);
			break;
		}
		case IDM_STEP_INTO: {
			*_TRAP = 0;
			theApp.zilog->execute(true);
			updateRegisters(_PC, U_REGS | U_PC | U_SP | U_SEL);
			break;
		}
		case IDM_STEP_OVER: {
			*_TRAP = 0;
			// если CALL nn/CALL ccc,nn -> выполняем до тех пор пока PC не станет следующей командой
			bool isCall = da->isCALL(sel);
			auto nextPC = da->move(_PC, 1);
			do {
				theApp.zilog->execute(true);
			} while(_PC != nextPC && isCall);
			updateRegisters(_PC, U_REGS | U_PC | U_SP | U_SEL);
			break;
		}
		case IDM_OVER_PROC: {
			*_TRAP = 0;
			auto exit_pc = *_PC_EXIT_CALL;
			while(_PC != exit_pc) theApp.zilog->execute(true);
			updateRegisters(_PC, U_REGS | U_PC | U_SP | U_SEL);
			break;
		}
		case IDM_PAUSE:
			setProgrammPause(true, false);
			break;
		case IDM_RUN:
			theApp.zilog->execute(true);
			setProgrammPause(false, false);
			break;
		case IDM_HEX_DEC:
			updateHexDec(true);
			break;
		case IDC_BUTTON_TO_PC:
			updateRegisters(_PC, U_PC | U_SEL | U_TOP);
			break;
		case IDC_BUTTON_TO_SP:
			updateStack(*_SP);
			break;
		case IDC_BUTTON_SET_COMMAND:
			break;
		case IDC_EDIT_S:
		case IDC_EDIT_Z:
		case IDC_EDIT_5:
		case IDC_EDIT_H:
		case IDC_EDIT_3:
		case IDC_EDIT_P:
		case IDC_EDIT_N:
		case IDC_EDIT_C:
			flag = true;
		case IDC_EDIT_BC:
		case IDC_EDIT_DE:
		case IDC_EDIT_HL:
		case IDC_EDIT_AF:
		case IDC_EDIT_BC_:
		case IDC_EDIT_DE_:
		case IDC_EDIT_HL_:
		case IDC_EDIT_AF_:
		case IDC_EDIT_IX:
		case IDC_EDIT_IY:
		case IDC_EDIT_SP:
		case IDC_EDIT_PC:
		case IDC_EDIT_R:
		case IDC_EDIT_I:
		case IDC_EDIT_IM:
		case IDC_EDIT_IFF1:
		case IDC_EDIT_IFF2:
//		case IDC_EDIT_STATE:
			switch(param) {
				case EN_CHANGE: {
					for(auto& zx : dlgElems) {
						if(zx.idMain == wmId) {
							GetWindowText(zx.hWndMain, tmpBuf, 256);
							bool dec = theApp.getOpt(OPT_DECIMAL)->dval;
							ssh_w v = (ssh_w)*(ssh_u*)(asm_ssh_wton(tmpBuf, (dec ? (ssh_u)Radix::_dec : (ssh_u)Radix::_hex)));
							if(zx.regb) {
								if(flag) {
									*zx.regb &= ~(1 << zx.shift);
									*zx.regb |= (v ? 1 : 0) << zx.shift;
								} else *zx.regb = (ssh_b)(v & zx.msk);
							} else if(zx.regw) {
								v &= zx.msk;
								*zx.regw = v;
							}
							break;
						}
					}
					break;
				}
			}
			break;
		case IDC_LIST_DA: {
			switch(param) {
				case LBN_VSCROLL: {
					int delta = (int)SendMessage(hWndDA, LB_GETTOPINDEX, 0, 0) - _pc;
					updateRegisters(da->move(_pc, delta), U_PC | U_TOP);
					break;
				}
				case LBN_DBLCLK: {
					// определить тип комманды с адресом перехода
					auto addr = da->getCmdOperand(sel, true);
					if(addr != -1) updateRegisters(addr, U_PC | U_STORY | U_SEL | U_TOP);
					break;
				}
				case LBN_SELCHANGE:
					SetWindowText(GetDlgItem(hWnd, IDC_EDIT_CURRENT_COMMAND), da->makeCommand(sel, theApp.getOpt(OPT_DECIMAL)->dval));
					break;
			}
			break;
		}
		case IDC_LIST_STACK:
			switch(param) {
				case LBN_DBLCLK: {
					ssh_w newPC = *(ssh_w*)(memZX + SendMessage((HWND)lParam, LB_GETCURSEL, 0, 0) * 2);
					updateRegisters(newPC, U_PC | U_STORY | U_SEL | U_TOP);
					break;
				}
			}
			break;
		case IDOK: break;
		default: return false;
	}
	return true;
}

INT_PTR zxDebugger::onCtlColorStatic(HDC hDC, HWND hWnd) {
	auto id = GetDlgCtrlID(hWnd);
	for(auto& zx : dlgElems) {
		if(zx.idText == id) {
			SetTextColor(hDC, zx.change ? RGB(255, 0, 0) : RGB(0, 0, 0));
			SetBkColor(hDC, GetSysColor(COLOR_BTNFACE));
			return (INT_PTR)::GetStockObject(LTGRAY_BRUSH);
		}
	}
	return (INT_PTR)0;
}

bool zxDebugger::check(ssh_w address, ZX_BREAK_POINT& breakpt, int flags) {
	for(auto& bp : bps) {
		if(bp.address1 == -1) continue;
		else if(address >= bp.address1 && address <= bp.address2 && (bp.flags & flags)) {
			breakpt = bp;
			return true;
		}
	}
	return false;
}

void zxDebugger::setProgrammPause(bool pause, bool activate) {
	if(toolbar) {
		int state1 = pause << 2;
		int state2 = (!pause) << 2;

		HWND hTB = toolbar->getHWND();
		SendMessage(hTB, TB_SETSTATE, IDM_PAUSE, state2);
		SendMessage(hTB, TB_SETSTATE, IDM_STEP_OVER, state1);
		SendMessage(hTB, TB_SETSTATE, IDM_STEP_INTO, state1);
		SendMessage(hTB, TB_SETSTATE, IDM_RUN, state1);
		SendMessage(hTB, TB_SETSTATE, IDM_OVER_PROC, state1);

		updateUndoRedo(false);
		updatePrevNextBP();

		modifyTSTATE(pause ? ZX_DEBUG : ZX_EXEC, pause ? ZX_EXEC : 0);
		if(pause) updateRegisters(_PC, U_PC | U_REGS | U_STORY | U_SEL | U_SP | U_TOP);
		theApp.changeExecute(false);

		if(activate) SetActiveWindow(hWnd);
	}
}

 bool zxDebugger::checkBPS(ssh_w address, bool mem) {
	static ZX_BREAK_POINT bp;
	bool res = false;
	int flags = (mem ? FBP_MEM : FBP_PC);
	if(check(address, bp, flags)) {
		if(flags == FBP_MEM) {
			if(bp.value != -1) {
				ssh_b v1 = (memZX[address] & bp.mask);
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
		} else res = true;
		if(res) {
			setProgrammPause(true, true);
		}
	}
	return res;
}

void zxDebugger::quickBP(int adr) {
	for(int i = 0; i < COUNT_BP; i++) {
		auto bp = &bps[i];
		if(bp->address1 == -1) {
			bp->address1 = adr;
			bp->address2 = adr;
			bp->value = -1;
			bp->mask = 0xff;
			bp->flags = FBP_PC;
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
		theApp.getOpt(i)->sval = StringZX::fmt(L"%d,%d,%d,%d", bp->address1, bp->address2, bp->value, bp->flags);
	}
}

void zxDebugger::removeBP(int index) {
	if(curIndexBP == index) curIndexBP--;
	memcpy(&bps[index], &bps[index + 1], (&bps[(COUNT_BP - 1)] - &bps[index]) * sizeof(ZX_BREAK_POINT));
	memset(&bps[(COUNT_BP - 1)], -1, sizeof(ZX_BREAK_POINT));
}