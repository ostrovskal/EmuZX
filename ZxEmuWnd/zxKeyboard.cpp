
#include "stdafx.h"
#include "zxKeyboard.h"
#include "zxGamePad.h"

// fe fd fb f7 ef df bf 7f
//	port[0xfefe] = 255;// CAPS_SHIFT Z X C V
//	port[0xfdfe] = 255;// A S D F G
//	port[0xfbfe] = 255;// Q W E R T 
//	port[0xf7fe] = 255;// 1 2 3 4 5
//	port[0xeffe] = 255;// 0 9 8 7 6
//	port[0xdffe] = 255;// P O I U Y
//	port[0xbffe] = 255;// ENTER L K J H
//	port[0x7ffe] = 255;// SPACE SYMBOL_SHIFT M N B
// DEL fefe-254,effe-254,bffe-254

BEGIN_MSG_MAP(zxKeyboard, zxDialog)
	ON_WM_CLOSE()
END_MSG_MAP()

static ZX_KEY_EX keysExDef[] = {
	{VK_BACK, '0', VK_CONTROL},
	{VK_LEFT, '5', VK_CONTROL},
	{VK_UP, '7', VK_CONTROL},
	{VK_RIGHT, '8', VK_CONTROL},
	{VK_DOWN, '6', VK_CONTROL},
	{VK_OEM_1, 'O', VK_SHIFT},
	{VK_OEM_PLUS, 'L', VK_SHIFT},
	{VK_OEM_COMMA, 'N', VK_SHIFT},
	{VK_OEM_MINUS, 'J', VK_SHIFT},
	{VK_OEM_PERIOD, 'M', VK_SHIFT},
	{VK_OEM_2, 'V', VK_SHIFT},
	{VK_OEM_4, 'Y', VK_SHIFT},
	{VK_OEM_5, 'D', VK_SHIFT},
	{VK_OEM_6, 'U', VK_SHIFT},
	{VK_OEM_7, '7', VK_SHIFT}
};

static ZX_KEY_EX keysExShift[] = {
	{VK_BACK, '0', VK_CONTROL},
	{VK_LEFT, '5', VK_CONTROL},
	{VK_UP, '7', VK_CONTROL},
	{VK_RIGHT, '8', VK_CONTROL},
	{VK_DOWN, '6', VK_CONTROL},
	{VK_OEM_1, 'Z', VK_SHIFT},
	{VK_OEM_PLUS, 'K', VK_SHIFT},
	{VK_OEM_COMMA, 'R', VK_SHIFT},
	{VK_OEM_MINUS, '0', VK_SHIFT},
	{VK_OEM_PERIOD, 'T', VK_SHIFT},
	{VK_OEM_2, 'C', VK_SHIFT},
	{VK_OEM_4, 'F', VK_SHIFT},
	{VK_OEM_5, 'S', VK_SHIFT},
	{VK_OEM_6, 'G', VK_SHIFT},
	{VK_OEM_7, 'P', VK_SHIFT},
	{'6', '7', VK_SHIFT},
	{'7', '6', VK_SHIFT},
	{'8', 'B', VK_SHIFT},
	{'9', '8', VK_SHIFT},
	{'0', '9', VK_SHIFT}
};

ZX_KEY keys[] = {
	{IDC_BUTTON_CAPS_LOCK},
	{IDC_BUTTON_K1, 3, 0x1, 0, 0, '1', L"1", L"1", L"1", L"blue", L"DEF FN", L"!", L"edit"},
	{IDC_BUTTON_K2, 3, 0x2, 0, 0, '2', L"2", L"2", L"2", L"red", L"FN", L"@", L"caps lock"},
	{IDC_BUTTON_K3, 3, 0x4, 0, 0, '3', L"3", L"3", L"3", L"magenta", L"LINE", L"#", L"tr video"},
	{IDC_BUTTON_K4, 3, 0x8, 0, 0, '4', L"4", L"4", L"4", L"green", L"OPEN", L"$", L"inv video"},
	{IDC_BUTTON_K5, 3, 0x10, 0, 0, '5', L"5", L"5", L"5", L"cyan", L"CLOSE", L"%", L"left"},
	{IDC_BUTTON_K6, 4, 0x10, 0, 0, '6', L"6", L"6", L"6", L"yellow", L"MOVE", L"&&", L"down"},
	{IDC_BUTTON_K7, 4, 0x8, 0, 0, '7', L"7", L"7", L"7", L"white", L"ERASE", L"'", L"right"},
	{IDC_BUTTON_K8, 4, 0x4, 0, 0, '8', L"8", L"8", L"8", L"", L"POINT", L"(", L"up"},
	{IDC_BUTTON_K9, 4, 0x2, 0, 0, '9', L"9", L"9", L"9", L"bright", L"CAT", L")", L"graphics"},
	{IDC_BUTTON_K0, 4, 0x1, 0, 0, '0', L"0", L"0", L"0", L"black", L"FORMAT", L"_", L"delete"},
	{IDC_BUTTON_KQ, 2, 0x1, 0, 0, 'Q', L"PLOT", L"q", L"Q", L"SIN", L"ASN", L"<=", L"Q"},
	{IDC_BUTTON_KW, 2, 0x2, 0, 0, 'W', L"DRAW", L"w", L"W", L"COS", L"ACS", L"<>", L"W"},
	{IDC_BUTTON_KE, 2, 0x4, 0, 0, 'E', L"REM", L"e", L"E", L"TAN", L"ATN", L">=", L"E"},
	{IDC_BUTTON_KR, 2, 0x8, 0, 0, 'R', L"RUN", L"r", L"R", L"INT", L"VERIFY", L"<", L"R"},
	{IDC_BUTTON_KT, 2, 0x10, 0, 0, 'T', L"RAND", L"t", L"T", L"RND", L"MERGE", L">", L"T"},
	{IDC_BUTTON_KY, 5, 0x10, 0, 0, 'Y', L"RETURN", L"y", L"Y", L"STR$", L"[", L"AND", L"Y"},
	{IDC_BUTTON_KU, 5, 0x8, 0, 0, 'U', L"IF", L"u", L"U", L"CHR$", L"]", L"OR", L"U"},
	{IDC_BUTTON_KI, 5, 0x4, 0, 0, 'I', L"INPUT", L"i", L"I", L"CODE", L"IN", L"AT", L"I"},
	{IDC_BUTTON_KO, 5, 0x2, 0, 0, 'O', L"POKE", L"o", L"O", L"PEEK", L"OUT", L";", L"O"},
	{IDC_BUTTON_KP, 5, 0x1, 0, 0, 'P', L"PRINT", L"p", L"P", L"TAB", L"@", L"\"", L"P"},
	{IDC_BUTTON_KA, 1, 0x1, 0, 0, 'A', L"NEW", L"a", L"A", L"READ", L"~", L"STOP", L"A"},
	{IDC_BUTTON_KS, 1, 0x2, 0, 0, 'S', L"SAVE", L"s", L"S", L"RESTORE", L"|", L"NOT", L"S"},
	{IDC_BUTTON_KD, 1, 0x4, 0, 0, 'D', L"DIM", L"d", L"D", L"DATE", L"\\", L"STEP", L"D"},
	{IDC_BUTTON_KF, 1, 0x8, 0, 0, 'F', L"FOR", L"f", L"F", L"SGN", L"{", L"TO", L"F"},
	{IDC_BUTTON_KG, 1, 0x10, 0, 0, 'G', L"GOTO", L"g", L"G", L"ABS", L"}", L"THEN", L"G"},
	{IDC_BUTTON_KH, 6, 0x10, 0, 0, 'H', L"GOSUB", L"h", L"H", L"SQR", L"CIRCLE", L"!", L"H"},
	{IDC_BUTTON_KJ, 6, 0x8, 0, 0, 'J', L"LOAD", L"j", L"J", L"VAL", L"VAL$", L"-", L"J"},
	{IDC_BUTTON_KK, 6, 0x4, 0, 0, 'K', L"LIST", L"k", L"K", L"LEN", L"SCRN$", L"+", L"K"},
	{IDC_BUTTON_KL, 6, 0x2, 0, 0, 'L', L"LET", L"l", L"L", L"USR", L"ATTR", L"=", L"L"},
	{IDC_BUTTON_ENTER, 6, 0x1, 0, 0, VK_RETURN, L"ENTER", L"ENTER", L"ENTER", L"ENTER", L"ENTER", L"ENTER", L"ENTER"},
	{IDC_BUTTON_CAPS_SHIFT, 0, 0x1, 0, 0, VK_CONTROL, L"CAPS SHIFT", L"CAPS SHIFT", L"CAPS SHIFT", L"CAPS SHIFT", L"CAPS SHIFT", L"CAPS SHIFT", L"CAPS SHIFT"},
	{IDC_BUTTON_KZ, 0, 0x2, 0, 0, 'Z', L"COPY", L"z", L"Z", L"LN", L"BEEP", L":", L"Z"},
	{IDC_BUTTON_KX, 0, 0x4, 0, 0, 'X', L"CLEAR", L"x", L"X", L"EXP", L"INK", L"FUNT", L"X"},
	{IDC_BUTTON_KC, 0, 0x8, 0, 0, 'C', L"CONT", L"c", L"C", L"LPRINT", L"PAPER", L"?", L"C"},
	{IDC_BUTTON_KV, 0, 0x10, 0, 0, 'V', L"CLS", L"v", L"V", L"LLIST", L"FLUSH", L"/", L"V"},
	{IDC_BUTTON_KB, 7, 0x10, 0, 0, 'B', L"BORDER", L"b", L"B", L"BIN", L"BRIGHT", L"*", L"B"},
	{IDC_BUTTON_KN, 7, 0x8, 0, 0, 'N', L"NEXT", L"n", L"N", L"INKEYS$", L"OVER", L",", L"N"},
	{IDC_BUTTON_KM, 7, 0x4, 0, 0, 'M', L"PAUSE", L"m", L"M", L"PI", L"INVERSE", L".", L"M"},
	{IDC_BUTTON_SYMBOL_SHIFT, 7, 0x2, 0, 0, VK_SHIFT, L"SYMBOL SHIFT", L"SYMBOL SHIFT", L"SYMBOL SHIFT", L"SYMBOL SHIFT", L"SYMBOL SHIFT", L"SYMBOL SHIFT", L"SYMBOL SHIFT"},
	{IDC_BUTTON_SPACE, 7, 0x1, 0, 0, VK_SPACE, L"", L"", L"", L"", L"", L"", L""},
	/*
	{0, 4, 0x8, 0, 1, VK_UP},
	{0, 4, 0x10, 0, 1, VK_DOWN},
	{0, 3, 0x10, 0, 1, VK_LEFT},
	{0, 4, 0x4, 0, 1, VK_RIGHT},
	{0, 31, 0x8, 0, 0, VK_UP},
	{0, 31, 0x1, 0, 0, VK_DOWN},
	{0, 31, 0x4, 0, 0, VK_LEFT},
	{0, 31, 0x2, 0, 0, VK_RIGHT},
	{0, 31, 0x10, 0, 0, '0'},
	*/
	{IDC_BUTTON_ENTER1, 6, 0x1, 0, 0, VK_RETURN, L"ENTER", L"ENTER", L"ENTER", L"ENTER", L"ENTER", L"ENTER", L"ENTER"},
};

int zxKeyboard::onInitDialog(HWND hWnd) {
	hFont = CreateFont(-8, 0, 0, 0, FW_THIN, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_TT_PRECIS,
					   CLIP_DEFAULT_PRECIS, PROOF_QUALITY, FF_ROMAN, L"Ms Shell Dlg");

	hbrSel = CreateSolidBrush(0x80000080);
	hbrUnsel = CreateSolidBrush(GetSysColor(COLOR_BTNFACE));

	for(auto& k : keys) {
		k.hWndKey = GetDlgItem(hWnd, k.butID);
		SendMessage(k.hWndKey, WM_SETFONT, (WPARAM)hFont, true);
	}

	return 1;
}

void zxKeyboard::show(bool visible) {
	if((bool)IsWindowVisible(hWnd) != visible) {
		ShowWindow(hWnd, visible);
		UpdateWindow(hWnd);
	}
}

void zxKeyboard::processKeys() {
	bool sh = (vkKeys[VK_SHIFT] & 0x80);
	bool caps = (vkKeys[VK_CONTROL] & 0x80);
	if(sh) {
		for(auto& k : keysExShift) {
			if(vkKeys[k.vk_code] & 0x80) {
				vkKeys[k.vk_code] = 0;
				vkKeys[k.vk_codeKey] = 0x80;
				vkKeys[k.vk_codeKeyEx] = 0x80;
				break;
			}
		}
	} else {
		for(auto& k : keysExDef) {
			if(vkKeys[k.vk_code] & 0x80) {
				vkKeys[k.vk_codeKey] = 0x80;
				vkKeys[k.vk_codeKeyEx] = 0x80;
			}
		}
	}
	for(auto& k : keys) {
		ssh_b bit = k.bit;
		if(vkKeys[k.vk_code] & 0x80) {
			_KEYS[k.port] &= ~bit;
		} else {
			_KEYS[k.port] |= bit;
		}
	}
	// проверить режим клавиатуры
	int nmode = KM_K;
	int val = memZX[23617], val1 = 0, val2 = 0;
	if(*_LOCK_FD || *_MODEL == MODEL_48K) {
		switch(val) {
			case 0:
				val1 = memZX[23658];
				val2 = memZX[23611];
				if(val2 & 8) {
					nmode = (val1 & 8) ? KM_C : KM_L;
				} else if(val1 & 16) nmode = KM_K;
				if(sh) nmode = KM_SH_KL;
				if(nmode == KM_L && caps) nmode = KM_CH;
				break;
			case 1: nmode = (sh ? KM_SH_E : KM_E); break;
			default: nmode = KM_G; break;
		}
	} else {
		nmode = (memZX[23611] & 8 ? ((memZX[23658] & 8) ? KM_C : KM_L) : KM_L);
		if(nmode == KM_L && caps) nmode = KM_CH;
	}
	// проверить на нажатые кнопки
	for(auto& k : keys) {
		if(!k.hWndKey) continue;
		ssh_cws name;
		switch(nmode) {
			case KM_K: name = k.name_k; break;
			case KM_L: name = k.name_l; break;
			case KM_C: name = k.name_c; break;
			case KM_E: name = k.name_e; break;
			case KM_G: name = k.name_l; break;
			case KM_SH_E: name = k.name_sh_e; break;
			case KM_SH_KL: name = k.name_sh_k; break;
			default: name = k.name_c; break;
		}
		if(k.butID == IDC_BUTTON_CAPS_LOCK)
			highlightKey(L"CAPS LOCK", &keys[0], (val1 & 8) || caps);
		else 
			highlightKey(name, &k, 0x80);
	}
}

void zxKeyboard::highlightKey(ssh_cws name, ZX_KEY* k, int isVirt) {
	RECT rect;
	auto h = k->hWndKey;
	bool is = (isVirt & 1);
	if(isVirt & 0x80) {
		is = !(_KEYS[k->port] & k->bit);
	}
	if(is) {
		auto hdc = GetDC(h);
		auto hfont = SelectObject(hdc, hFont);
		SetTextColor(hdc, RGB(255, 255, 255));
		GetClientRect(h, &rect);
		SetBkMode(hdc, TRANSPARENT);
		FillRect(hdc, &rect, hbrSel);
		DrawText(hdc, name, -1, &rect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
		ReleaseDC(h, hdc);
	} else {
		SetWindowText(h, name);
	}
}

void zxKeyboard::processJoystick() {
	return;
	auto pad = theApp->gamepad;
	pad->update();
	if(pad->is_connected(0)) pad->remap(0);
	if(pad->is_connected(1)) pad->remap(1);
	if(pad->is_connected(2)) pad->remap(2);
	if(pad->is_connected(3)) pad->remap(3);
}
