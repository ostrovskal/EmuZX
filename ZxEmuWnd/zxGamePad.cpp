
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
	ssh_memzero(map, sizeof(map));
	ssh_memzero(modes, sizeof(modes));
	ssh_memzero(types, sizeof(types));
	ssh_memzero(joys, sizeof(joys));
}

void zxGamepad::changeMode(ssh_d idx, Mode m, Remapping* remapData, int count) {
	static Remapping kempston[5] = {{31, 8},{31, 1},{31, 4},{31, 2},{31, 16}};
	static Remapping sinclair[5] = {{239, 2}, {239, 8}, {239, 4}, {239, 16}, {239, 1}};
	
	ssh_memzero(&map[idx][0], sizeof(Remapping) * 16);
	switch(m) {
		case KEMPSTON:
			memcpy(&map[idx][0], &kempston, sizeof(Remapping) * 5);
			break;
		case SINCLAIR:
			memcpy(&map[idx][0], &sinclair, sizeof(Remapping) * 5);
			break;
		default:
			memcpy(&map[idx][0], remapData, sizeof(Remapping) * count);
			break;
	}
	modes[idx] = m;
}

void zxGamepad::remap(ssh_d idx, Buttons but) {
	auto m = &map[idx][but];
	auto bit = m->bit;
	auto is = is_pressed(idx, but);
	if(modes[idx] == Mode::KEMPSTON) {
		auto val = *_KEMPSTON;
		if(is) val |= bit; else val &= ~bit;
		*_KEMPSTON = val;
	} else {
		auto val = _KEYS[m->port];
		if(is) val &= ~bit; else val |= bit;
		_KEYS[m->port] = val;
	}
}
