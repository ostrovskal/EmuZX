#include "stdafx.h"
#include "zxDlgSettings.h"
#include "zxGamePad.h"

static SHEET_DATA sheetCommon[] = {
	{IDC_SLIDER_PERIOD_CPU, OPT_DELAY_CPU, (ssh_cws)0x0210},
	{IDC_SLIDER_PERIOD_GPU, OPT_DELAY_GPU, (ssh_cws)0x0a28},
	{IDC_SLIDER_PERIOD_BORDER, OPT_PERIOD_BORDER, (ssh_cws)0x0820},
	{IDC_SLIDER_PERIOD_BLINK, OPT_PERIOD_BLINK, (ssh_cws)0x0208},
	{IDC_CHECK_WRITE_ROM, OPT_WRITE_ROM},
	{IDC_CHECK_AUTO_SAVE, OPT_AUTO_SAVE},
};

static SHEET_DATA sheetSnd[] = {
	{IDC_SLIDER_VOL_BEEPER, OPT_SND_BEEPER_VOL},
	{IDC_SLIDER_VOL_COVOX, OPT_SND_COVOX_VOL},
	{IDC_SLIDER_VOL_AY, OPT_SND_AY_VOL},
	{IDC_SLIDER_VOL_SPECDRUM, OPT_SND_SPECDRUM_VOL},

	{IDC_CHECK_AY, OPT_SND_AY},
	{IDC_CHECK_COVOX, OPT_SND_COVOX},
	{IDC_CHECK_SPECDRUM, OPT_SND_SPECDRUM},
	{IDC_CHECK_BEEPER, OPT_SND_BEEPER},
	{IDC_CHECK_MIC, OPT_SND_MIC},
	{IDC_CHECK_8BIT, OPT_SND_8BIT},

	{IDC_COMBO_STEREO_AY, OPT_SND_AY_STEREO, L"None;ABC;ACB"},
	{IDC_COMBO_SND_FREQ, OPT_SND_FREQUENCY, L"22050;44100;48000"},
};

static SHEET_DATA sheetGpu[] = {
	{IDC_BUTTON_BLACK, OPT_COLOR_BLACK, L"Черный"},
	{IDC_BUTTON_BLUE, OPT_COLOR_BLUE, L"Синий"},
	{IDC_BUTTON_RED, OPT_COLOR_RED, L"Красный"},
	{IDC_BUTTON_MAGENTA, OPT_COLOR_MAGENTA, L"Розовый"},
	{IDC_BUTTON_GREEN, OPT_COLOR_GREEN, L"Зелёный"},
	{IDC_BUTTON_CYAN, OPT_COLOR_CYAN, L"Голубой"},
	{IDC_BUTTON_YELLOW, OPT_COLOR_YELLOW, L"Жёлтый"},
	{IDC_BUTTON_WHITE, OPT_COLOR_WHITE, L"Белый"},
	{IDC_BUTTON_BLACK2, OPT_COLOR_L_BLACK, L"Св. чёрный"},
	{IDC_BUTTON_BLUE2, OPT_COLOR_L_BLUE, L"Св. синий"},
	{IDC_BUTTON_RED2, OPT_COLOR_L_RED, L"Св. красный"},
	{IDC_BUTTON_MAGENTA2, OPT_COLOR_L_MAGENTA, L"Св. розовый"},
	{IDC_BUTTON_GREEN2, OPT_COLOR_L_GREEN, L"Св. зеленый"},
	{IDC_BUTTON_CYAN2, OPT_COLOR_L_CYAN, L"Св. голубой"},
	{IDC_BUTTON_YELLOW2, OPT_COLOR_L_YELLOW, L"Св. жёлтый"},
	{IDC_BUTTON_WHITE2, OPT_COLOR_L_WHITE, L"Св. белый"},

	{IDC_SLIDER_BLUE, 0},
	{IDC_SLIDER_GREEN, 1},
	{IDC_SLIDER_RED, 2},
};

static SHEET_DATA sheetJoy[] = {
	{IDC_COMBO_JOY_NUMBER, 0, L"№ 1;№ 2;№ 3;№ 4"},
	{IDC_COMBO_JOY_MAPPING, 0, L"NOTHING;KEMPSTON;SINCLAIR 1;SINCLAIR 2;CURSOR;KEYBOARD;CUSTOM"},
	{IDC_COMBO_AXIS_X, 0, L"X +;X -"},
	{IDC_COMBO_AXIS_Y, 0, L"Y +;Y -"},
	{IDC_COMBO_AXIS_Z, 0, L"Z +;Z -"},
	{IDC_COMBO_ROT_X, 0, L"X +;X -"},
	{IDC_COMBO_ROT_Y, 0, L"Y +;Y -"},
	{IDC_COMBO_ROT_Z, 0, L"Z +;Z -"},
	{IDC_SPLIT_POV_UP, zxGamepad::povUp},
	{IDC_SPLIT_POV_DOWN, zxGamepad::povDown},
	{IDC_SPLIT_POV_LEFT, zxGamepad::povLeft},
	{IDC_SPLIT_POV_RIGHT, zxGamepad::povRight},
	{IDC_SPLIT_AXIS_X, zxGamepad::xAxisP},
	{IDC_SPLIT_AXIS_Y, zxGamepad::yAxisP},
	{IDC_SPLIT_AXIS_Z, zxGamepad::zAxisP},
	{IDC_SPLIT_ROT_X, zxGamepad::xRotP},
	{IDC_SPLIT_ROT_Y, zxGamepad::yRotP},
	{IDC_SPLIT_ROT_Z, zxGamepad::zRotP},
	{IDC_SPLIT_BUT1, zxGamepad::but1},
	{IDC_SPLIT_BUT2, zxGamepad::but2},
	{IDC_SPLIT_BUT3, zxGamepad::but3},
	{IDC_SPLIT_BUT4, zxGamepad::but4},
	{IDC_SPLIT_BUT5, zxGamepad::but5},
	{IDC_SPLIT_BUT6, zxGamepad::but6},
	{IDC_SPLIT_BUT7, zxGamepad::but7},
	{IDC_SPLIT_BUT8, zxGamepad::but8},
	{IDC_SPLIT_BUT9, zxGamepad::but9},
	{IDC_SPLIT_BUT10, zxGamepad::but10},
	{IDC_SPLIT_BUT11, zxGamepad::but11},
	{IDC_SPLIT_BUT12, zxGamepad::but12},
};


BEGIN_MSG_MAP(zxDlgSheetGpu, zxDialog)
	ON_WM_HSCROLL()
	ON_CONTROL_RANGE(BN_CLICKED, IDC_BUTTON_BLACK, IDC_BUTTON_WHITE2, onButtonColor)
	ON_WM_DRAWITEM()
END_MSG_MAP()

BEGIN_MSG_MAP(zxDlgSheetJoystick, zxDialog)
	ON_NOTIFY_RANGE(BCN_DROPDOWN, IDC_SPLIT_POV_UP, IDC_SPLIT_BUT12, onSplitButtonDropdown)
	ON_CONTROL(CBN_SELCHANGE, IDC_COMBO_JOY_NUMBER, onComboJoyNumber)
	ON_CONTROL(CBN_SELCHANGE, IDC_COMBO_JOY_MAPPING, onComboJoyMapping)
	ON_CONTROL_RANGE(CBN_SELCHANGE, IDC_COMBO_AXIS_X, IDC_COMBO_AXIS_Z, onComboJoyAxis)
	ON_CONTROL_RANGE(CBN_SELCHANGE, IDC_COMBO_ROT_X, IDC_COMBO_ROT_Z, onComboJoyRot)
END_MSG_MAP()

BEGIN_MSG_MAP(zxDlgSettings, zxDialog)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_SETTINGS, onNotifyTabSettings)
END_MSG_MAP()

int zxDlgSheetCommon::onInitDialog(HWND hWnd) {
	for(int i = 0; i < 6; i++) {
		auto k = &sheetCommon[i];
		auto val = theApp->getOpt(k->index)->dval;
		HWND h = GetDlgItem(hWnd, k->id);
		k->hWnd = h;
		if(i < 4) {
			ssh_u items = (ssh_u)(k->items);
			SendMessage(h, TBM_SETRANGE, 0, MAKELONG((items & 0xff00) >> 8, items & 0xff));
			SendMessage(h, TBM_SETPOS, TRUE, val);
		} else {
			SendMessage(h, BM_SETCHECK, (val ? BST_CHECKED : BST_UNCHECKED), 0);
		}
	}

	return 1;
}

int zxDlgSheetSound::onInitDialog(HWND hWnd) {
	for(int i = 0; i < 12; i++) {
		auto k = &sheetSnd[i];
		auto val = theApp->getOpt(k->index)->dval;
		auto h = GetDlgItem(hWnd, k->id);
		k->hWnd = h;
		if(i < 4) {
			SendMessage(h, TBM_SETRANGE, 0, MAKELONG(0, 100));
			SendMessage(h, TBM_SETPOS, TRUE, val);
			SendMessage(h, TBM_SETTICFREQ, 4, 0);

		} else if(i < 10) {
			SendMessage(h, BM_SETCHECK, (val ? BST_CHECKED : BST_UNCHECKED), 0);
		} else {
			int count;
			auto items = zxString(k->items).split(L";", count);
			for(int j = 0 ; j < count; j++) SendMessage(h, CB_ADDSTRING, 0, (LPARAM)items[j].str());
			SendMessage(h, CB_SETCURSEL, val, 0);
		}
	}
	return 1;
}

void zxDlgSheetGpu::onDrawItem(int id, LPDRAWITEMSTRUCT dis) {
	RECT rect;
	HDC hdc = dis->hDC;
	auto k = &sheetGpu[id - IDC_BUTTON_BLACK];
	ssh_d col = k->data;
	col = ((col & 0x000000ff) << 16) | (col & 0x0000ff00) | ((col & 0x00ff0000) >> 16);
	ssh_d invCol = col ^ (0x00ffffff);
	auto hbr = CreateSolidBrush(col);

	SelectObject(hdc, hFont);
	SetTextColor(hdc, invCol);
	GetClientRect(dis->hwndItem, &rect);
	SetBkMode(hdc, TRANSPARENT);
	FillRect(hdc, &rect, hbr);
	DrawText(hdc, k->items, -1, &rect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
	if(stkCurrent && dis->hwndItem == stkCurrent->hWnd) {
		auto hpen = CreatePen(PS_DOT, 1, invCol);
		SelectObject(hdc, hpen);
		POINT pt;
		MoveToEx(hdc, rect.left + 2, rect.top + 2, &pt);
		LineTo(hdc, rect.right - 3, rect.top + 2);
		LineTo(hdc, rect.right - 3, rect.bottom - 3);
		LineTo(hdc, rect.left + 2, rect.bottom - 3);
		LineTo(hdc, rect.left + 2, rect.top + 2);
		DeleteObject(hpen);
	}
	DeleteObject(hbr);
}

void zxDlgSheetGpu::onHScroll(UINT code, UINT p, HWND hWndScroll) {
	int blue	= (int)SendMessage(sheetGpu[16].hWnd, TBM_GETPOS, 0, 0);
	int green	= (int)SendMessage(sheetGpu[17].hWnd, TBM_GETPOS, 0, 0) << 8;
	int red		= (int)SendMessage(sheetGpu[18].hWnd, TBM_GETPOS, 0, 0) << 16;
	stkCurrent->data = 0xff000000 | blue | green | red;
	InvalidateRect(stkCurrent->hWnd, nullptr, FALSE);
}

void zxDlgSheetGpu::onButtonColor() {
	auto k = &sheetGpu[wmId - IDC_BUTTON_BLACK];
	stkCurrent = k;
	auto pos = k->data;
	SendMessage(sheetGpu[16].hWnd, TBM_SETPOS, TRUE, pos & 0xff);
	SendMessage(sheetGpu[17].hWnd, TBM_SETPOS, TRUE, (pos & 0xff00) >> 8);
	SendMessage(sheetGpu[18].hWnd, TBM_SETPOS, TRUE, (pos & 0xff0000) >> 16);
	InvalidateRect(hWnd, nullptr, FALSE);
}

int zxDlgSheetGpu::onInitDialog(HWND hWnd) {
	hFont = CreateFont(-8, 0, 0, 0, FW_THIN, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_TT_PRECIS,
					   CLIP_DEFAULT_PRECIS, PROOF_QUALITY, FF_ROMAN, L"Ms Shell Dlg");
	for(int i = 0 ; i < 19; i++) {
		auto& k = sheetGpu[i];
		HWND h = GetDlgItem(hWnd, k.id);
		k.hWnd = h;
		if(i > 15) {
			SendMessage(h, TBM_SETRANGE, FALSE, MAKELONG(0, 255));
			SendMessage(h, TBM_SETTICFREQ, 4, 0);
		} else {
			k.data = theApp->getOpt(k.index)->dval;
		}
	}
	stkCurrent = &sheetGpu[0];
	SetFocus(stkCurrent->hWnd);

	return 1;
}

void zxDlgSheetJoystick::onComboJoyNumber() {

}

void zxDlgSheetJoystick::onComboJoyMapping() {

}

void zxDlgSheetJoystick::onComboJoyAxis() {

}

void zxDlgSheetJoystick::onComboJoyRot() {

}

int zxDlgSheetJoystick::onInitDialog(HWND hWnd) {
	for(auto& j : sheetJoy) {
		j.hWnd = GetDlgItem(hWnd, j.id);
		if(j.items) {
			zxString its(j.items);
			int count = 0;
			auto items = its.split(L";", count);
			for(int i = 0; i < count; i++) {
				SendMessage(j.hWnd, CB_ADDSTRING, 0, (LPARAM)items[i].str());
			}
		}
	}
	return 1;
}

void zxDlgSheetJoystick::onSplitButtonDropdown(NMHDR* nm, LRESULT* res) {
	TPMPARAMS tpm;
	RECT rc;

	GetWindowRect(GetDlgItem(hWnd, wmId), &rc);
	tpm.cbSize = sizeof(TPMPARAMS);
	tpm.rcExclude = rc;

	auto hMenu = LoadMenu(hInst, MAKEINTRESOURCE(IDR_MENU_JOYSTICK));
	auto hPopup = GetSubMenu(hMenu, 0);

	TrackPopupMenuEx(hPopup, TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_VERTICAL, rc.left, rc.bottom, hWnd, &tpm);

	DestroyMenu(hMenu);
}

int zxDlgSettings::onInitDialog(HWND hWnd) {
	RECT rc;
	TCITEM tci;
	tci.mask = TCIF_TEXT;

	hWndTab = GetDlgItem(hWnd, IDC_TAB_SETTINGS);
	
	tci.pszText = L"Общие";
	TabCtrl_InsertItem(hWndTab, 0, &tci);
	tci.pszText = L"Графика";
	TabCtrl_InsertItem(hWndTab, 1, &tci);
	tci.pszText = L"Звук";
	TabCtrl_InsertItem(hWndTab, 2, &tci);
	tci.pszText = L"Джойстик";
	TabCtrl_InsertItem(hWndTab, 3, &tci);

	GetClientRect(hWndTab, &rc);

	pageCommon.create(IDD_PAGE_COMMON, hWndTab, false);
	pageSound.create(IDD_PAGE_SOUND, hWndTab, false);
	pageGpu.create(IDD_PAGE_GPU, hWndTab, false);
	pageJoystick.create(IDD_PAGE_JOYSTICK, hWndTab, false);
	MoveWindow(pageCommon.getHWND(), 2, 21, rc.right - 5, rc.bottom - 25, false);
	MoveWindow(pageSound.getHWND(), 2, 21, rc.right - 5, rc.bottom - 25, false);
	MoveWindow(pageGpu.getHWND(), 2, 21, rc.right - 5, rc.bottom - 25, false);
	MoveWindow(pageJoystick.getHWND(), 2, 21, rc.right - 5, rc.bottom - 25, false);
	
	pageCurrent = &pageCommon;
	pageCommon.showWindow(true);

	return 1;
}

void zxDlgSettings::onNotifyTabSettings(NMHDR*, LRESULT*) {
	auto indexTab = SendMessage(hWndTab, TCM_GETCURSEL, 0, 0);
	auto pageOld = pageCurrent;
	switch(indexTab) {
		case 0: pageCurrent = &pageCommon; break;
		case 1: pageCurrent = &pageGpu; break;
		case 2: pageCurrent = &pageSound; break;
		case 3: pageCurrent = &pageJoystick; break;
		default: pageCurrent = nullptr;
	}
	if(pageCurrent) {
		if(pageCurrent != pageOld) {
			if(pageOld) pageOld->showWindow(false);
			pageCurrent->showWindow(true);
			pageCurrent->updateWindow();
		}
	}
}

void zxDlgSheetCommon::onOK() {
	for(int i = 0; i < 6; i++) {
		auto k = &sheetCommon[i];
		theApp->getOpt(k->index)->dval = (ssh_d)SendMessage(k->hWnd, (i < 4 ? TBM_GETPOS : BM_GETCHECK), 0, 0);
	}
	zxDialog::onOK();
}

void zxDlgSheetSound::onOK() {
	for(int i = 0; i < 12; i++) {
		auto k = &sheetSnd[i];
		theApp->getOpt(k->index)->dval = (ssh_d)SendMessage(k->hWnd, (i < 4 ? TBM_GETPOS : (i < 10 ? BM_GETCHECK : CB_GETCURSEL)), 0, 0);
	}
	zxDialog::onOK();
}

void zxDlgSheetGpu::onOK() {
	for(auto& k : sheetGpu) theApp->getOpt(k.index)->dval = k.data;
	zxDialog::onOK();
}

void zxDlgSheetJoystick::onOK() {
	for(int i = 0; i < zxGamepad::COUNT_CONTROLLERS; i++) {
		zxString val;
		for(int j = 0; j < 28; j++) {
			_itow_s(map[i][j], tmpBuf, 260, 16);
			val += tmpBuf;
			if(j < 27) val += L',';
		}
		theApp->getOpt(i + OPT_JOY1_MAPPING)->sval = val;
		theApp->getOpt(i + OPT_JOY1_STATUS)->dval = status[i];
	}
	zxDialog::onOK();
}

void zxDlgSettings::onOK() {
	pageCommon.onOK();
	pageSound.onOK();
	pageGpu.onOK();
	pageJoystick.onOK();
	//if(theApp->getOpt(OPT_AUTO_SAVE)->dval) theApp->opts.save()
	theApp->bus.updateData();
	zxDialog::onOK();
}

