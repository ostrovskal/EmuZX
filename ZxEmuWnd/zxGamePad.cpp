
#include "stdafx.h"
#include "zxGamePad.h"
#include "zxKeyboard.h"

extern zxKeyboard::ZX_KEY keys[];

zxGamepad::zxGamepad() {
	memset(wNowButtons, 0, sizeof(wNowButtons));
	memset(wButtons, 0, sizeof(wButtons));
	memset(axis, 0, sizeof(axis));
	memset(rot, 0, sizeof(rot));
	memset(connected, 0, sizeof(connected));
	memset(inserted, 0, sizeof(inserted));
	memset(removed, 0, sizeof(removed));
	memset(mapOrig, 0, sizeof(mapOrig));
	memset(mapOrig, 0, sizeof(mapOrig));
	memset(modes, 0, sizeof(modes));
}

void zxGamepad::init() {
	// установить режим
	for(int i = 0; i < 2; i++) {
		zxString map = theApp->getOpt(OPT_JOY1_STATUS + i)->sval;
		// POV4, but12, xAxis2, yAxis2, zAxis2, xRot2, yRot2, zRot2
		int count = 0;
		auto maps = map.split(L",", count);
		memset(&mapOrig[i], 0, countButtons);
		if(count > countButtons) count = countButtons;
		if(count < 0) count = 0;
		for(int j = 0; j < count; j++) {
			mapOrig[i][j] = (ssh_b)*(ssh_u*)asm_ssh_wton(maps[j].str(), (ssh_u)Radix::_hex);
		}
		changeMode(i, theApp->getOpt(OPT_JOY1_MAPPING + i)->dval);
	}
}

void zxGamepad::update() {
	JOYINFOEX jie;
	jie.dwSize = sizeof(JOYINFOEX);
	for(int i = 0; i < 2; i++) {
		bool is = (joyGetPosEx(i, &jie) == JOYERR_NOERROR);
		bool bWasConnected(connected[i]);
		connected[i] = is;
		removed[i] = (bWasConnected && !connected[i]);
		inserted[i] = (!bWasConnected && connected[i]);
		if(is) {
			wButtons[i] = wNowButtons[i];
			axis[i][axisX] = (jie.dwXpos - 32767); axis[i][axisY] = jie.dwYpos - 32767; axis[i][axisZ] = jie.dwZpos - 32767;
			rot[i][axisX] = jie.dwRpos - 32767; rot[i][axisY] = jie.dwUpos - 32767; rot[i][axisZ] = jie.dwVpos - 32767;
			ssh_w v = 0;
			if(jie.dwFlags & JOY_RETURNPOV) {
				// POV
				auto pov = jie.dwPOV;
				if(pov != 0xffff) {
					static ssh_b cnvPOV[8] = {1, 3, 2, 6, 4, 12, 8, 9};
					v |= cnvPOV[pov / 4500];
				}
			}
			if(jie.dwFlags & JOY_RETURNBUTTONS) {
				// BUTTONS 12
				v |= (jie.dwButtons << 4) & 0xffff;
			}
			wNowButtons[i] = v;
		}
	}
}

ssh_b* zxGamepad::getPredefinedMode(int mode, int& count) {
	static ssh_b kempston[5]	= {zxKeyboard::zxKeyboard::KEY_KEM_UP, zxKeyboard::KEY_KEM_RIGHT, zxKeyboard::KEY_KEM_DOWN,
									zxKeyboard::KEY_KEM_LEFT, zxKeyboard::KEY_KEM_FIRE};
	static ssh_b sinclair1[5]	= {zxKeyboard::KEY_8, zxKeyboard::KEY_7, zxKeyboard::KEY_9, zxKeyboard::KEY_6, zxKeyboard::KEY_0};
	static ssh_b sinclair2[5]	= {zxKeyboard::KEY_3, zxKeyboard::KEY_2, zxKeyboard::KEY_4, zxKeyboard::KEY_1, zxKeyboard::KEY_5};
	static ssh_b cursor[5]		= {zxKeyboard::KEY_UP, zxKeyboard::KEY_RIGHT, zxKeyboard::KEY_DOWN, zxKeyboard::KEY_LEFT, zxKeyboard::KEY_0};
	static ssh_b keyboard[9]	= {zxKeyboard::KEY_Q, zxKeyboard::KEY_P, zxKeyboard::KEY_A, zxKeyboard::KEY_O, zxKeyboard::KEY_SPACE,
									zxKeyboard::KEY_M, zxKeyboard::KEY_D, zxKeyboard::KEY_S, zxKeyboard::KEY_E};

	count = 5;
	
	switch(mode) {
		case JOY_KEMPSTON: return kempston;
		case JOY_SINCLAIR_I: return sinclair1;
		case JOY_SINCLAIR_II: return sinclair2;
		case JOY_CURSOR: return cursor;
		case JOY_KEYBOARD: count = 9; return keyboard;
	}
	return nullptr;
}

void zxGamepad::changeMode(bool second, int mode) {
	int count = 0;
	auto m = getPredefinedMode(mode, count);
	modes[second] = mode;
	memcpy(&mapCurrent[second], &mapOrig[second], countButtons);
	if(m) memcpy(&mapCurrent[second], m, count);
}

void zxGamepad::updateKey(bool pressed, int k) {
	zxKeyboard::ZX_KEY* key = &keys[k];
	if(key->bitEx) {
		ssh_b portEx = key->portEx;
		ssh_b bitEx = key->bitEx;
		if(pressed) _KEYS[portEx] &= ~bitEx; else _KEYS[portEx] |= bitEx;
	}
	ssh_b port = key->port;
	ssh_b bit = key->bit;
	if(port == 31) {
		if(pressed) (*_KEMPSTON) |= bit; else (*_KEMPSTON) &= ~bit;
	} else {
		if(pressed) _KEYS[port] &= ~bit; else _KEYS[port] |= bit;
	}
}

void zxGamepad::remap(bool second) {
	// проверить все кнопки джойстика и установить/сбросить соответствующие им кнопки на клаве
	for(int i = 0; i < countButtons; i++) {
		ssh_b k = mapCurrent[second][i];
		if(k == 0) continue;
		if(i > but12) {
			// проверить оси и повороты
			long val;
			if(i > zAxisM) {
				//xRotP, xRotM, yRotP, yRotM, zRotP, zRotM
				val = is_rotate(second, (Axis)(i - xRotP));
			} else {
				//xAxisP, xAxisM, yAxisP, yAxisM, zAxisP, zAxisM
				val = is_axis(second, (Axis)((i - xAxisP) / 2));
			}
			if(val > 0 && !(i & 1)) {
				updateKey(true, k);
			} else if(val < 0 && (i & 1)) {
				updateKey(true, k);
			} else {
				updateKey(false, k);
			}
		} else {
			updateKey(wButtons[second] & (1 << i), k);
		}
	}
}
