
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

ZX_KEY_EX keysExDef[] = {
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

ZX_KEY_EX keysExShift[] = {
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

static ZX_KEY keys[] = {
	{IDC_BUTTON_CAPS_LOCK},
	{IDC_BUTTON_K1, L"1", L"1", L"1", L"blue", L"DEF FN", L"!", L"edit", 3, 0x1, '1'},
	{IDC_BUTTON_K2, L"2", L"2", L"2", L"red", L"FN", L"@", L"caps lock", 3, 0x2, '2'},
	{IDC_BUTTON_K3, L"3", L"3", L"3", L"magenta", L"LINE", L"#", L"tr video", 3, 0x4, '3'},
	{IDC_BUTTON_K4, L"4", L"4", L"4", L"green", L"OPEN", L"$", L"inv video", 3, 0x8, '4'},
	{IDC_BUTTON_K5, L"5", L"5", L"5", L"cyan", L"CLOSE", L"%", L"left", 3, 0x10, '5'},
	{IDC_BUTTON_K6, L"6", L"6", L"6", L"yellow", L"MOVE", L"&&", L"down", 4, 0x10, '6'},
	{IDC_BUTTON_K7, L"7", L"7", L"7", L"white", L"ERASE", L"'", L"right", 4, 0x8, '7'},
	{IDC_BUTTON_K8, L"8", L"8", L"8", L"", L"POINT", L"(", L"up", 4, 0x4, '8'},
	{IDC_BUTTON_K9, L"9", L"9", L"9", L"bright", L"CAT", L")", L"graphics", 4, 0x2, '9'},
	{IDC_BUTTON_K0, L"0", L"0", L"0", L"black", L"FORMAT", L"_", L"delete", 4, 0x1, '0'},
	{IDC_BUTTON_KQ, L"PLOT", L"q", L"Q", L"SIN", L"ASN", L"<=", L"Q", 2, 0x1, 'Q'},
	{IDC_BUTTON_KW, L"DRAW", L"w", L"W", L"COS", L"ACS", L"<>", L"W", 2, 0x2, 'W'},
	{IDC_BUTTON_KE, L"REM", L"e", L"E", L"TAN", L"ATN", L">=", L"E", 2, 0x4, 'E'},
	{IDC_BUTTON_KR, L"RUN", L"r", L"R", L"INT", L"VERIFY", L"<", L"R", 2, 0x8, 'R'},
	{IDC_BUTTON_KT, L"RAND", L"t", L"T", L"RND", L"MERGE", L">", L"T", 2, 0x10, 'T'},
	{IDC_BUTTON_KY, L"RETURN", L"y", L"Y", L"STR$", L"[", L"AND", L"Y", 5, 0x10, 'Y'},
	{IDC_BUTTON_KU, L"IF", L"u", L"U", L"CHR$", L"]", L"OR", L"U", 5, 0x8, 'U'},
	{IDC_BUTTON_KI, L"INPUT", L"i", L"I", L"CODE", L"IN", L"AT", L"I", 5, 0x4, 'I'},
	{IDC_BUTTON_KO, L"POKE", L"o", L"O", L"PEEK", L"OUT", L";", L"O", 5, 0x2, 'O'},
	{IDC_BUTTON_KP, L"PRINT", L"p", L"P", L"TAB", L"@", L"\"", L"P", 5, 0x1, 'P'},
	{IDC_BUTTON_KA, L"NEW", L"a", L"A", L"READ", L"~", L"STOP", L"A", 1, 0x1, 'A'},
	{IDC_BUTTON_KS, L"SAVE", L"s", L"S", L"RESTORE", L"|", L"NOT", L"S", 1, 0x2, 'S'},
	{IDC_BUTTON_KD, L"DIM", L"d", L"D", L"DATE", L"\\", L"STEP", L"D", 1, 0x4, 'D'},
	{IDC_BUTTON_KF, L"FOR", L"f", L"F", L"SGN", L"{", L"TO", L"F", 1, 0x8, 'F'},
	{IDC_BUTTON_KG, L"GOTO", L"g", L"G", L"ABS", L"}", L"THEN", L"G", 1, 0x10, 'G'},
	{IDC_BUTTON_KH, L"GOSUB", L"h", L"H", L"SQR", L"CIRCLE", L"!", L"H", 6, 0x10, 'H'},
	{IDC_BUTTON_KJ, L"LOAD", L"j", L"J", L"VAL", L"VAL$", L"-", L"J", 6, 0x8, 'J'},
	{IDC_BUTTON_KK, L"LIST", L"k", L"K", L"LEN", L"SCRN$", L"+", L"K", 6, 0x4, 'K'},
	{IDC_BUTTON_KL, L"LET", L"l", L"L", L"USR", L"ATTR", L"=", L"L", 6, 0x2, 'L'},
	{IDC_BUTTON_ENTER, L"ENTER", L"ENTER", L"ENTER", L"ENTER", L"ENTER", L"ENTER", L"ENTER", 6, 0x1, VK_RETURN},
	{IDC_BUTTON_ENTER1, L"ENTER", L"ENTER", L"ENTER", L"ENTER", L"ENTER", L"ENTER", L"ENTER", 6, 0x1, VK_RETURN},
	{IDC_BUTTON_CAPS_SHIFT, L"CAPS SHIFT", L"CAPS SHIFT", L"CAPS SHIFT", L"CAPS SHIFT", L"CAPS SHIFT", L"CAPS SHIFT", L"CAPS SHIFT", 0, 0x1, VK_CONTROL},
	{IDC_BUTTON_KZ, L"COPY", L"z", L"Z", L"LN", L"BEEP", L":", L"Z", 0, 0x2, 'Z'},
	{IDC_BUTTON_KX, L"CLEAR", L"x", L"X", L"EXP", L"INK", L"FUNT", L"X", 0, 0x4, 'X'},
	{IDC_BUTTON_KC, L"CONT", L"c", L"C", L"LPRINT", L"PAPER", L"?", L"C", 0, 0x8, 'C'},
	{IDC_BUTTON_KV, L"CLS", L"v", L"V", L"LLIST", L"FLUSH", L"/", L"V", 0, 0x10, 'V'},
	{IDC_BUTTON_KB, L"BORDER", L"b", L"B", L"BIN", L"BRIGHT", L"*", L"B", 7, 0x10, 'B'},
	{IDC_BUTTON_KN, L"NEXT", L"n", L"N", L"INKEYS$", L"OVER", L",", L"N", 7, 0x8, 'N'},
	{IDC_BUTTON_KM, L"PAUSE", L"m", L"M", L"PI", L"INVERSE", L".", L"M", 7, 0x4, 'M'},
	{IDC_BUTTON_SYMBOL_SHIFT, L"SYMBOL SHIFT", L"SYMBOL SHIFT", L"SYMBOL SHIFT", L"SYMBOL SHIFT", L"SYMBOL SHIFT", L"SYMBOL SHIFT", L"SYMBOL SHIFT", 7, 0x2, VK_SHIFT},
	{IDC_BUTTON_SPACE, L"", L"", L"", L"", L"", L"", L"", 7, 0x1, VK_SPACE},
};

void zxKeyboard::postCreate() {
	for(auto& k : keys) {
		k.hWndKey = GetDlgItem(hWnd, k.butID);
		SendMessage(k.hWndKey, WM_SETFONT, (WPARAM)hFont, true);
	}
}

bool zxKeyboard::preCreate() {
	hFont = CreateFont(-8, 0, 0, 0, FW_THIN, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_TT_PRECIS,
					   CLIP_DEFAULT_PRECIS, PROOF_QUALITY, FF_ROMAN, L"Ms Shell Dlg");

	hbrSel = CreateSolidBrush(RGB(255, 0, 0));
	hbrUnsel = CreateSolidBrush(GetSysColor(COLOR_BTNFACE));
	return true;
}

void zxKeyboard::show(bool visible) {
	ShowWindow(hWnd, visible);
	UpdateWindow(hWnd);
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
				break;
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
	int nmode = -1;
	int val = memZX[23617], val1 = 0, val2 = 0;
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
	// проверить на нажатые кнопки
	for(auto& k : keys) {
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
		if(name) highlightKey(name, &k, 0x80);
		highlightKey(L"CAPS LOCK", &keys[0], ((val1 & 8) || caps) ? 1 : 0);
	}
}

void zxKeyboard::highlightKey(ssh_cws name, ZX_KEY* k, int isVirt) {
	RECT rect;
	bool is = (isVirt & 1);
	if(isVirt & 0x80) {
		is = !(_KEYS[k->port] & k->bit);
	}
	auto h = k->hWndKey;
	auto hdc = GetDC(h);
	auto hfont = SelectObject(hdc, hFont);
	SetTextColor(hdc, is ? RGB(255, 255, 255) : RGB(0, 0, 0));
	GetClientRect(h, &rect);
	SetBkMode(hdc, TRANSPARENT);
	FillRect(hdc, &rect, is ? hbrSel : hbrUnsel);
	DrawText(hdc, name, -1, &rect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
	ReleaseDC(h, hdc);
}

void zxKeyboard::processJoystick() {
	auto pad = theApp->gamepad;
	pad->update();
	if(pad->is_connected(0)) {
		pad->remap(0, zxGamepad::crossUp);
		pad->remap(0, zxGamepad::crossDown);
		pad->remap(0, zxGamepad::crossLeft);
		pad->remap(0, zxGamepad::crossRight);
		pad->remap(0, zxGamepad::A);
	}
}
