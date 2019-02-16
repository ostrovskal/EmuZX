
#include "stdafx.h"
#include "zxGamePad.h"

static LPDIRECTINPUT8			pDI = nullptr;
static DIJOYSTATE2				state;
static IDirectInputJoyConfig8*	pJoyCfg = nullptr;
static int joyIndex				= 0;

BOOL CALLBACK EnumJoys(const DIDEVICEINSTANCE* pdidInstance, VOID* wnd) {
	return ((zxGamepad*)wnd)->create(pdidInstance);
}

BOOL CALLBACK EnumObjs(const DIDEVICEOBJECTINSTANCE* pdidoi, VOID* wnd) {
	return ((zxGamepad*)wnd)->enumObjs(pdidoi);
}

BOOL zxGamepad::enumObjs(const DIDEVICEOBJECTINSTANCE* pdidoi) {
	Types* tp = nullptr;
	HRESULT hr;

	auto joy = joys[joyIndex];

	// подсчитать кол-во кнопок, осей по типу, крестовин
	if(pdidoi->dwType & DIDFT_AXIS) {
		DIPROPRANGE diprg;
		diprg.diph.dwSize = sizeof(DIPROPRANGE);
		diprg.diph.dwHeaderSize = sizeof(DIPROPHEADER);
		diprg.diph.dwHow = DIPH_BYID;
		diprg.diph.dwObj = pdidoi->dwType;
		diprg.lMin = -100;
		diprg.lMax = +100;
		hr = joy->SetProperty(DIPROP_RANGE, &diprg.diph);
	} else if(pdidoi->dwType & DIDFT_POV) {
		DIPROPDWORD didw;
		didw.diph.dwSize = sizeof(DIPROPDWORD);
		didw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
		didw.diph.dwHow = DIPH_BYID;
		didw.diph.dwObj = pdidoi->dwType;
		hr = joy->GetProperty(DIPROP_GRANULARITY, &didw.diph);
		if(SUCCEEDED(hr)) granPOV[joyIndex] = didw.dwData;
	}
	if(pdidoi->guidType == GUID_XAxis)
		tp = &types[joyIndex][xAxis];
	if(pdidoi->guidType == GUID_YAxis)
		tp = &types[joyIndex][yAxis];
	if(pdidoi->guidType == GUID_ZAxis)
		tp = &types[joyIndex][zAxis];
	if(pdidoi->guidType == GUID_RxAxis)
		tp = &types[joyIndex][RxAxis];
	if(pdidoi->guidType == GUID_RyAxis)
		tp = &types[joyIndex][RyAxis];
	if(pdidoi->guidType == GUID_RzAxis)
		tp = &types[joyIndex][RzAxis];
	if(pdidoi->guidType == GUID_Slider)
		tp = &types[joyIndex][Slider];
	if(pdidoi->guidType == GUID_POV)
		tp = &types[joyIndex][POV];
	if(pdidoi->guidType == GUID_Button)
		tp = &types[joyIndex][Button];
	if(tp) { int* t = (int*)tp; (*t)++; }
	return DIENUM_CONTINUE;
}

BOOL zxGamepad::create(const DIDEVICEINSTANCE* pdidInstance) {
	try {
		DIJOYCONFIG	joyCfg = {0};
		joyCfg.dwSize = sizeof(DIJOYCONFIG);
		if(SUCCEEDED(pJoyCfg->GetConfig(joyIndex, &joyCfg, DIJC_GUIDINSTANCE))) {
			auto hwc = &joyCfg.hwc;
			if(IsEqualGUID(pdidInstance->guidInstance, joyCfg.guidInstance)) {
				if(SUCCEEDED(pDI->CreateDevice(pdidInstance->guidInstance, &joys[joyIndex], nullptr))) {
					if(FAILED(joys[joyIndex]->SetDataFormat(&c_dfDIJoystick2))) throw(3);
					if(FAILED(joys[joyIndex]->SetCooperativeLevel(theApp->getHWND(), DISCL_EXCLUSIVE | DISCL_FOREGROUND))) throw(4);
					if(FAILED(joys[joyIndex]->EnumObjects(EnumObjs, this, DIDFT_ALL))) throw(5);
					connected[joyIndex] = true;
					joyIndex++;
				}
			}
		}
	} catch(...) {
		return DIENUM_STOP;
	}
	return DIENUM_CONTINUE;
}

bool zxGamepad::init(HWND hWnd) {
	bool result = true;
	try {
		if(FAILED(DirectInput8Create(GetModuleHandle(NULL), DIRECTINPUT_VERSION, IID_IDirectInput8, (VOID**)&pDI, nullptr)))
			return false;

		joyIndex = 0;

		if(FAILED(pDI->QueryInterface(IID_IDirectInputJoyConfig8, (void**)&pJoyCfg))) throw(0);
		if(FAILED(pDI->EnumDevices(DI8DEVCLASS_GAMECTRL, EnumJoys, this, DIEDFL_ATTACHEDONLY))) throw(1);

		// установить режим
		for(int i = 0; i < COUNT_CONTROLLERS; i++) {
			zxString map = theApp->getOpt(OPT_JOY1_MAPPING + i)->sval;
			// POV4, but12, xAxis2, yAxis2, zAxis2, xRot2, yRot2, zRot2
			int count = 0;
			auto maps = map.split(L",", count);
			memset(&mapOrig[i], 0, countButtons);
			
			if(count > countButtons) count = countButtons;
			if(count < 0) count = 0;
			for(int j = 0; j < count; j++) {
				mapOrig[i][j] = (ssh_b)*(ssh_u*)asm_ssh_wton(maps[j].str(), (ssh_u)Radix::_hex);// _wtol(maps[j].str());
			}
			changeMode(i, theApp->getOpt(OPT_JOY1_STATUS + i)->dval);
		}

	} catch(...) {
		cleanup();
		reset();
		result = false;
	}

	SAFE_RELEASE(pJoyCfg);

	return result;
}

void zxGamepad::cleanup() {
	for(int i = 0; i < COUNT_CONTROLLERS; i++) {
		if(joys[i]) {
			joys[i]->Unacquire();
			SAFE_RELEASE(joys[i]);
		}
	}
	SAFE_RELEASE(pDI);
}

void zxGamepad::update() {
	for(int i = 0; i < COUNT_CONTROLLERS; i++) {
		bool bWasConnected(connected[i]);
		bool is = (joys[i] != nullptr);
		connected[i] = is;
		removed[i] = (bWasConnected && !connected[i]);
		inserted[i] = (!bWasConnected && connected[i]);
		if(!connected[i]) continue;
		auto joy = joys[i];
		if(FAILED(joy->Poll())) {
			HRESULT hr(joy->Acquire());
			while(hr == DIERR_INPUTLOST) hr = joy->Acquire();
			return;
		}
		if(SUCCEEDED(joy->GetDeviceState(sizeof(DIJOYSTATE2), &state))) {
			axis[i][axisX] = state.lX; axis[i][axisY] = state.lY; axis[i][axisZ] = state.lZ;
			rot[i][axisX] = state.lRx; rot[i][axisY] = state.lRy; rot[i][axisZ] = state.lRz;
			wButtons[i] = wNowButtons[i];
			ssh_d pov = state.rgdwPOV[0];
			ssh_w v = 0;
			if(pov != -1) {
				static ssh_b cnvPOV[8] = {1, 3, 2, 6, 4, 12, 8, 9};
				v |= cnvPOV[pov / granPOV[i]];
			}
			for(int j = 4; j < 16; j++) {
				v |= (((state.rgbButtons[j - 4] & 0x80) != 0) << j);
			}
			wNowButtons[i] = v;
		}
	}
}

void zxGamepad::reset() {
	// все стереть
	ssh_memzero(wNowButtons, sizeof(wNowButtons));
	ssh_memzero(wButtons, sizeof(wButtons));
	ssh_memzero(axis, sizeof(axis));
	ssh_memzero(rot, sizeof(rot));
	ssh_memzero(connected, sizeof(connected));
	ssh_memzero(inserted, sizeof(inserted));
	ssh_memzero(removed, sizeof(removed));
	ssh_memzero(granPOV, sizeof(granPOV));
	ssh_memzero(modes, sizeof(modes));
	ssh_memzero(types, sizeof(types));
	ssh_memzero(joys, sizeof(joys));
}

void zxGamepad::changeMode(ssh_d idx, int mode) {
	static ssh_b kempston[5] = {ZX_KEY_KEM_UP, ZX_KEY_KEM_DOWN, ZX_KEY_KEM_LEFT, ZX_KEY_KEM_RIGHT, ZX_KEY_KEM_FIRE};
	static ssh_b sinclair1[5] = {ZX_KEY_8, ZX_KEY_9, ZX_KEY_6, ZX_KEY_7, ZX_KEY_0};
	static ssh_b sinclair2[5] = {ZX_KEY_3, ZX_KEY_4, ZX_KEY_1, ZX_KEY_2, ZX_KEY_5};
	static ssh_b cursor[5] = {ZX_KEY_UP, ZX_KEY_DOWN, ZX_KEY_LEFT, ZX_KEY_RIGHT, ZX_KEY_0};
	static ssh_b keyboard[9] = {ZX_KEY_Q, ZX_KEY_A, ZX_KEY_O, ZX_KEY_P, ZX_KEY_SPACE, ZX_KEY_M, ZX_KEY_D, ZX_KEY_S, ZX_KEY_E};

	ssh_b* m = nullptr;
	int count = 5;
	switch(mode) {
		case JOY_KEMPSTON: m = kempston; break;
		case JOY_INTERFACE_I: m = sinclair1; break;
		case JOY_INTERFACE_II: m = sinclair2; break;
		case JOY_CURSOR: m = cursor; break;
		case JOY_KEYBOARD: m = keyboard; count = 9; break;
	}
	modes[idx] = mode;
	memcpy(&mapCurrent[idx], &mapOrig[idx], countButtons);
	if(m) memcpy(&mapCurrent[idx], m, count);
}

void zxGamepad::updateKey(bool pressed, int k) {
	ZX_KEY* key = &keys[k];
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

void zxGamepad::remap(ssh_d idx) {
	// проверить все кнопки джойстика и установить/сбросить соответствующие им кнопки на клаве
	for(int i = 0; i < countButtons; i++) {
		ssh_b k = mapCurrent[idx][i];
		if(k == 0) continue;
		if(i > but12) {
			// проверить оси и повороты
			long val;
			if(i > zAxisM) {
				//xRotP, xRotM, yRotP, yRotM, zRotP, zRotM
				val = is_rotate(idx, (Axis)(i / 2 - xRotP));
			} else {
				//xAxisP, xAxisM, yAxisP, yAxisM, zAxisP, zAxisM
				val = is_axis(idx, (Axis)(i / 2 - xAxisP));
			}
			if(val > 0 && (i & 1)) {
				updateKey(true, k);
			} else if(val < 0 && !(i & 1)) {
				updateKey(true, k);
			} else {
				updateKey(false, k);
			}
		} else {
			updateKey(wButtons[i] & (1 << i), k);
		}
	}
}
