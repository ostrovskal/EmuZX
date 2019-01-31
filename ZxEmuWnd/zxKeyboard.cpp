
#include "stdafx.h"
#include "zxKeyboard.h"

static ZX_KEY keys[] = {
	{IDC_BUTTON_K1, L"1", L"1", L"1", L"blue", L"DEF FN", L"!", L"edit", 0xfe, 0x1},
	{IDC_BUTTON_K2, L"2", L"2", L"2", L"red", L"FN", L"@", L"caps lock", 0xfe, 0x2},
	{IDC_BUTTON_K3, L"3", L"3", L"3", L"magenta", L"LINE", L"#", L"tr video", 0xfe, 0x4},
	{IDC_BUTTON_K4, L"4", L"4", L"4", L"green", L"OPEN", L"$", L"inv video", 0xfe, 0x8},
	{IDC_BUTTON_K5, L"5", L"5", L"5", L"cyan", L"CLOSE", L"%", L"left", 0xfe, 0x10},
	{IDC_BUTTON_K6, L"6", L"6", L"6", L"yellow", L"MOVE", L"'", L"down", 0xef, 0x10},
	{IDC_BUTTON_K7, L"7", L"7", L"7", L"white", L"ERASE", L"&&", L"right", 0xef, 0x8},
	{IDC_BUTTON_K8, L"8", L"8", L"8", L"", L"POINT", L"*", L"up", 0xef, 0x4},
	{IDC_BUTTON_K9, L"9", L"9", L"9", L"bright", L"CAT", L"(", L"graphics", 0xef, 0x2},
	{IDC_BUTTON_K0, L"0", L"0", L"0", L"black", L"FORMAT", L")", L"delete", 0xef, 0x1},
	{IDC_BUTTON_KQ, L"PLOT", L"q", L"Q", L"SIN", L"ASN", L"<=", L"Q", 0xfb, 0x1},
	{IDC_BUTTON_KW, L"DRAW", L"w", L"W", L"COS", L"ACS", L"<>", L"W", 0xfb, 0x2},
	{IDC_BUTTON_KE, L"REM", L"e", L"E", L"TAN", L"ATN", L">=", L"E", 0xfb, 0x4},
	{IDC_BUTTON_KR, L"RUN", L"r", L"R", L"INT", L"VERIFY", L"<", L"R", 0xfb, 0x8},
	{IDC_BUTTON_KT, L"RAND", L"t", L"T", L"RND", L"MERGE", L">", L"T", 0xfb, 0x10},
	{IDC_BUTTON_KY, L"RETURN", L"y", L"Y", L"STR$", L"[", L"AND", L"Y", 0xdf, 0x10},
	{IDC_BUTTON_KU, L"IF", L"u", L"U", L"CHR$", L"]", L"OR", L"U", 0xdf, 0x8},
	{IDC_BUTTON_KI, L"INPUT", L"i", L"I", L"CODE", L"IN", L"AT", L"I", 0xdf, 0x4},
	{IDC_BUTTON_KO, L"POKE", L"o", L"O", L"PEEK", L"OUT", L";", L"O", 0xdf, 0x2},
	{IDC_BUTTON_KP, L"PRINT", L"p", L"P", L"TAB", L"@", L"\"", L"P", 0xdf, 0x1},
	{IDC_BUTTON_KA, L"NEW", L"a", L"A", L"READ", L"~", L"STOP", L"A", 0xfd, 0x1},
	{IDC_BUTTON_KS, L"SAVE", L"s", L"S", L"RESTORE", L"|", L"NOT", L"S", 0xfd, 0x2},
	{IDC_BUTTON_KD, L"DIM", L"d", L"D", L"DATE", L"\\", L"STEP", L"D", 0xfd, 0x4},
	{IDC_BUTTON_KF, L"FOR", L"f", L"F", L"SGN", L"{", L"TO", L"F", 0xfd, 0x8},
	{IDC_BUTTON_KG, L"GOTO", L"g", L"G", L"ABS", L"}", L"THEN", L"G", 0xfd, 0x10},
	{IDC_BUTTON_KH, L"GOSUB", L"h", L"H", L"SQR", L"CIRCLE", L"!", L"H", 0xbf, 0x10},
	{IDC_BUTTON_KJ, L"LOAD", L"j", L"J", L"VAL", L"VAL$", L"-", L"J", 0xbf, 0x8},
	{IDC_BUTTON_KK, L"LIST", L"k", L"K", L"LEN", L"SCRN$", L"+", L"K", 0xbf, 0x4},
	{IDC_BUTTON_KL, L"LET", L"l", L"L", L"USR", L"ATTR", L"=", L"L", 0xbf, 0x2},
	{IDC_BUTTON_ENTER, L"ENTER", L"ENTER", L"ENTER", L"ENTER", L"ENTER", L"ENTER", L"ENTER", 0xbf, 0x1},
	{IDC_BUTTON_ENTER1, L"ENTER", L"ENTER", L"ENTER", L"ENTER", L"ENTER", L"ENTER", L"ENTER", 0xbf, 0x1},
	{IDC_BUTTON_CAPS_SHIFT, L"CAPS SHIFT", L"CAPS SHIFT", L"CAPS SHIFT", L"CAPS SHIFT", L"CAPS SHIFT", L"CAPS SHIFT", L"CAPS SHIFT", 0xbf, 0x1},
	{IDC_BUTTON_KZ, L"COPY", L"z", L"Z", L"LN", L"BEEP", L":", L"Z", 0xfe, 0x2},
	{IDC_BUTTON_KX, L"CLEAR", L"x", L"X", L"EXP", L"INK", L"FUNT", L"X", 0xfe, 0x4},
	{IDC_BUTTON_KC, L"CONT", L"c", L"C", L"LPRINT", L"PAPER", L"?", L"C", 0xfe, 0x8},
	{IDC_BUTTON_KV, L"CLS", L"v", L"V", L"LLIST", L"FLUSH", L"/", L"V", 0xfe, 0x10},
	{IDC_BUTTON_KB, L"BORDER", L"b", L"B", L"BIN", L"BRIGHT", L"*", L"B", 0x7f, 0x10},
	{IDC_BUTTON_KN, L"NEXT", L"n", L"N", L"INKEYS$", L"OVER", L",", L"N", 0x7f, 0x8},
	{IDC_BUTTON_KM, L"PAUSE", L"m", L"M", L"PI", L"INVERSE", L".", L"M", 0x7f, 0x4},
	{IDC_BUTTON_SYMBOL_SHIFT, L"SYMBOL SHIFT", L"SYMBOL SHIFT", L"SYMBOL SHIFT", L"SYMBOL SHIFT", L"SYMBOL SHIFT", L"SYMBOL SHIFT", L"SYMBOL SHIFT", 0x7f, 0x2},
	{IDC_BUTTON_SPACE, L"", L"", L"", L"", L"", L"", L"", 0x7f, 0x1}
};

static ssh_d WINAPI KeyProc(void* params) {
	return theApp.keyboard->procKEY();
}

DWORD zxKeyboard::procKEY() {
	int val, val1, val2;
	LARGE_INTEGER sample;

	QueryPerformanceFrequency(&sample);
	DWORD ms = sample.LowPart / 1000;

	QueryPerformanceCounter(&sample);
	DWORD start = sample.LowPart / ms;

	while(!(_TSTATE & ZX_TERMINATE)) {
		QueryPerformanceCounter(&sample);
		DWORD millis = sample.LowPart / ms;
		if((millis - start) < 10) continue;
		start = millis;
		// проверить режим клавиатуры
		int nmode = -1;
		if(!memZX) continue;
		val = memZX[23617];
		int sh = (GetKeyState(VK_SHIFT) & 128);
		switch(val) {
			case 0:
				val1 = memZX[23658];
				val2 = memZX[23611];
				if(val1 & 8) nmode = KM_C; else { if(val2 & 8) nmode = KM_L; else if(val1 & 16) nmode = KM_K; }
				if(sh) nmode = KM_SH_KL;
				if(nmode == KM_L && (GetKeyState(VK_CONTROL) & 128)) nmode = KM_CH;
				break;
			case 1: nmode = (sh ? KM_SH_E : KM_E); break;
			default: nmode = KM_G; break;
		}
		// проверить на нажатые кнопки
		if(mode == nmode) continue;
		for(auto& k : keys) {
			int p = ((k.port << 8) | 0xfe);
			auto v = portsZX[p];
			//if(!(v & k.bit)) SendMessage(k.hWndKey, BM_CLICK, 0, 0);
			ssh_cws name = nullptr;
			switch(nmode) {
				case KM_K: name = k.name_k; break;
				case KM_L: name = k.name_l; break;
				case KM_C: name = k.name_c; break;
				case KM_E: name = k.name_e; break;
				case KM_G: name = k.name_l; break;
				case KM_SH_E: name = k.name_sh_e; break;
				case KM_SH_KL: name = k.name_sh_k; break;
				case KM_CH: name = k.name_c; break;
			}
			if(nmode != mode && name != nullptr) SetWindowText(k.hWndKey, name);
		}
		mode = nmode;
	}
	return 0;
}

void zxKeyboard::onInitDialog(HWND hWnd, LPARAM lParam) {
	for(auto& k : keys) {
		k.hWndKey = GetDlgItem(hWnd, k.butID);
		SendMessage(k.hWndKey, WM_SETFONT, (WPARAM)hFont, true);
	}
}

bool zxKeyboard::preCreate() {
	DWORD keyID;
	hKeyThread = CreateThread(nullptr, 0, KeyProc, nullptr, CREATE_SUSPENDED, &keyID);
	hFont = CreateFont(-10, 0, 0, 0, FW_THIN, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_TT_PRECIS,
					   CLIP_DEFAULT_PRECIS, PROOF_QUALITY, FF_ROMAN, L"Courier New");

	hbrSel = CreateSolidBrush(RGB(255, 0, 0));
	hbrUnsel = CreateSolidBrush(GetSysColor(COLOR_BTNFACE));
	return true;
}

void zxKeyboard::show(bool visible) {
	if(visible) {
		showWindow(true);
		updateWindow();
		ResumeThread(hKeyThread);
	} else {
		if(isWindowVisible()) {
			showWindow(false);
			Wow64SuspendThread(hKeyThread);
		}
	}

}

bool zxKeyboard::onDrawItem(UINT idCtl, LPDRAWITEMSTRUCT dis) {
	auto isStat = (dis->itemState & ODS_SELECTED) != 0;
	if(isStat) {
		isStat = isStat;
	}
	GetWindowText(dis->hwndItem, tmpBuf, 260);
	StringZX txt(tmpBuf);
	RECT* r = &dis->rcItem;
	HDC hdc = dis->hDC;
	POINT pt;

	SetTextColor(hdc, GetSysColor(isStat ? COLOR_HIGHLIGHTTEXT : COLOR_BTNTEXT));
	SetBkMode(hdc, TRANSPARENT);
	FillRect(hdc, r, isStat ? hbrSel : hbrUnsel);
	DrawText(hdc, txt, (int)txt.length(), r, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	MoveToEx(hdc, r->left, r->top, &pt);
	LineTo(hdc, r->right - 1, r->top);
	LineTo(hdc, r->right - 1, r->bottom - 1);
	LineTo(hdc, r->left, r->bottom - 1);
	LineTo(hdc, r->left, r->top);
	return true;
}
