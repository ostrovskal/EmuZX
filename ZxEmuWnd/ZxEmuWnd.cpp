// ZxEmuWnd.cpp: Определяет точку входа для приложения.
//

#include "stdafx.h"
#include "ZxEmuWnd.h"
#include "ZxTimer.h"
#include "GpuZX.h"
#include "KeyboardZX.h"
#include "BorderZX.h"
#include "SoundZX.h"
#include "SettingsZX.h"
#include <commdlg.h>

CpuZX* zilog	= nullptr;
GpuZX* gpu		= nullptr;
BorderZX* brd	= nullptr;
KeyboardZX* keyb= nullptr;
SoundZX* snd	= nullptr;
SettingsZX settings;

HWND hWnd;
HWND hToolbar;
HINSTANCE hInst;
RECT rectWnd;
HMENU hMenu;
MENUITEMINFO mii;
HMENU hMenuMRU;

DWORD delayCPU;
DWORD delayGPU;

DWORD WINAPI ProcCPU(void* params) {
	ZxTimer tm;

	tm.start();
	
	DWORD sndTm = 0;
	DWORD startCpu = tm.samplePerformanceCounter();
	DWORD startBrd = startCpu;
	DWORD startGpu = startCpu / tm.millisecondsFrequency;

	while(true) {
		DWORD current = tm.samplePerformanceCounter();
		DWORD millis = current / tm.millisecondsFrequency;
		if((current - startCpu) > delayCPU) { 
			zilog->execute(); startCpu = current; 
			if(settings.isSound) snd->execute(sndTm++);
		}
		if((millis - startGpu) > delayGPU) { gpu->execute(); startGpu = millis; }
		if((current - startBrd) > (delayCPU * 16)) { brd->execute(); startBrd = current; }
		
	}
	tm.stop();
}

bool dlgSaveOrOpenFile(bool bOpen, ssh_cws title, ssh_cws filter, ssh_cws defExt, StringZX& folder) {
	bool result;
	static ssh_ws flt[MAX_PATH];

	int flags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY;

	OPENFILENAME ofn;
	memset(&ofn, 0, sizeof(OPENFILENAME));

	if(bOpen) flags |= OFN_FILEMUSTEXIST;

	auto files = new ssh_ws[MAX_PATH];
	memset(files, 0, MAX_PATH);

	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hWnd;

	ofn.lpstrDefExt = defExt;
	ofn.lpstrFilter = filter;
	ofn.lpstrInitialDir = folder;
	ofn.lpstrTitle = title;
	ofn.lpstrFile = files;
	ofn.lpstrFileTitle = nullptr;
	ofn.nFilterIndex = 1;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = flags;

	if(bOpen)
		result = ::GetOpenFileName(&ofn);
	else
		result = ::GetSaveFileName(&ofn);

	if(result) folder = ofn.lpstrFile;

	delete files;

	return result;
}

extern ssh_cws nameROMs[];

void SetMenuState(bool* val, int id, bool change, bool invert = false) {
	if(change) *val = !*val;
	bool v = *val;
	if(invert) v = !v;
	CheckMenuItem(hMenu, id, v << 3);
	SendMessage(hToolbar, TB_SETSTATE, id, TBSTATE_ENABLED | (int)v);
}

void SetTurboMode(bool change) {
	SetMenuState(&settings.isTurbo, IDM_TURBO, change);
	delayCPU = settings.delayCPU;
	delayGPU = settings.delayGPU;
	if(settings.isTurbo) {
		delayCPU /= 2;
		delayGPU /= 2;
	}
}

void CheckedMenuModel(HMENU hMenu, int rom) {
	static int mid[] = {IDM_48K, IDM_128K};
	for(int nid : mid) CheckMenuItem(hMenu, nid, 0);
	CheckMenuItem(hMenu, mid[rom], MF_CHECKED);
	if(settings.modelZX != rom) {
		settings.modelZX = rom;
		zilog->signalRESET();
	}
}

void ModifyMRU(StringZX path, bool insert) {
	if(!path.is_empty()) {
		int i;
		for(i = 0; i < 9; i++) if(settings.mru[i] == path) break;
		int n = i;
		for(; i > 0; i--) settings.mru[i] = settings.mru[i - 1];
		settings.mru[0] = path;

		mii.cbSize = sizeof(MENUITEMINFO);
		mii.fMask = MIIM_TYPE;
		mii.fType = MFT_STRING;
		mii.cch = (UINT)path.length();
		mii.dwTypeData = path.buffer();

		RemoveMenu(hMenuMRU, IDM_MRU_EMPTY, MF_BYCOMMAND);
		RemoveMenu(hMenuMRU, n, MF_BYPOSITION);
		InsertMenuItem(hMenuMRU, 0, TRUE, &mii);
		auto count = GetMenuItemCount(hMenuMRU);
		for(int i = 0; i < count; i++) {
			mii.fMask = MIIM_ID;
			mii.wID = 1000 + i;
			SetMenuItemInfo(hMenuMRU, i, TRUE, &mii);
		}
	}
}

void makeToolbar() {
	
	TBBUTTON tbb[] = {
		{0, IDM_OPEN, TBSTATE_ENABLED, BTNS_AUTOSIZE, {0}, 0, (INT_PTR)L"Открыть"},
		{1, IDM_SAVE, TBSTATE_ENABLED, BTNS_AUTOSIZE, {0}, 0, (INT_PTR)L"Сохранить"},
		{10, -1, TBSTATE_ENABLED, BTNS_SEP, {0}, 0, 0},
		{2, IDM_RESET, TBSTATE_ENABLED, BTNS_AUTOSIZE, {0}, 0, (INT_PTR)L"Сброс"},
		{10, -1, TBSTATE_ENABLED, BTNS_SEP, {0}, 0, 0},
		{3, IDM_RESTORE, TBSTATE_ENABLED, BTNS_AUTOSIZE, {0}, 0, (INT_PTR)L"Восстановить состояние"},
		{10, -1, TBSTATE_ENABLED, BTNS_SEP, {0}, 0, 0},
		{4, IDM_PAUSE, TBSTATE_ENABLED, BTNS_AUTOSIZE, {0}, 0, (INT_PTR)L"Остановить\\Возобновить эмуляцию"},
		{5, IDM_SOUND, TBSTATE_ENABLED, BTNS_AUTOSIZE, {0}, 0, (INT_PTR)L"Включить\\Выключить звук"},
		{6, IDM_KEYBOARD, TBSTATE_ENABLED, BTNS_AUTOSIZE, {0}, 0, (INT_PTR)L"Показать\\Скрыть клавиатуру"},
		{7, IDM_MONITOR, TBSTATE_ENABLED, BTNS_AUTOSIZE, {0}, 0, (INT_PTR)L"Показать\\Скрыть дизассемблер"},
		{10, -1, TBSTATE_ENABLED, BTNS_SEP, {0}, 0, 0},
		{8, IDM_TURBO, TBSTATE_ENABLED, BTNS_AUTOSIZE, {0}, 0, (INT_PTR)L"Ускорить\\Замедлить"},
		{10, -1, TBSTATE_ENABLED, BTNS_SEP, {0}, 0, 0},
		{9, IDM_MODEL, TBSTATE_ENABLED, BTNS_WHOLEDROPDOWN | BTNS_AUTOSIZE, {0}, 0, (INT_PTR)L"Модель памяти"},
		{10, -1, TBSTATE_ENABLED, BTNS_SEP, {0}, 0, 0},
		{10, IDM_SETTINGS, TBSTATE_ENABLED, BTNS_AUTOSIZE, {0}, 0, (INT_PTR)L"Настройки"},
	};

	auto hBmp = LoadBitmap(hInst, (LPCWSTR)IDB_TOOLBAR_COMMON);

	hToolbar = CreateToolbarEx(hWnd, TBSTYLE_TOOLTIPS | WS_CLIPSIBLINGS | WS_CHILD | WS_VISIBLE | CCS_TOP, -1, 15, NULL, (UINT_PTR)hBmp, tbb, 15, 32, 32, 352, 32, sizeof(TBBUTTON));

	SendMessage(hToolbar, TB_SETEXTENDEDSTYLE, 0, TBSTYLE_EX_DRAWDDARROWS);
	SendMessage(hToolbar, TB_SETMAXTEXTROWS, 0, 0);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch(message) {
		case WM_COMMAND: {
			int wmId = LOWORD(wParam);
			StringZX folder(settings.curPath);
			// Разобрать выбор в меню:
			switch(wmId) {
				case 1000: case 1001: case 1002: case 1003: case 1004:
				case 1005: case 1006: case 1007: case 1008: case 1009:
					mii.fMask = MIIM_TYPE;
					mii.fType = MFT_STRING;
					mii.cch = MAX_PATH;
					if(!GetMenuItemInfo(hMenuMRU, wmId, FALSE, &mii)) break;
					folder = mii.dwTypeData;
					wmId = 1;
				case IDM_OPEN:
					if(wmId == IDM_OPEN) {
						wmId = dlgSaveOrOpenFile(true, L"Открыть", L"Снепшот\0*.z80\0Состояние\0*.zx\0", L"z80", folder);
					}
					if(wmId) {
						settings.curPath = folder.left(folder.find_rev(L'\\') + 1);
						StringZX ext(folder.substr(folder.find_rev(L'.') + 1).lower());
						bool result = false;
						if(ext == L"z80") {
							result = zilog->loadZ80(folder);
						} else if(ext == L"zx") {
							result = zilog->loadStateZX(folder);
						}
						if(!result) MessageBox(hWnd, L"Не удалось выполнить операцию!", L"Ошибка", MB_OK);
						else ModifyMRU(folder, true);
					}
					break;
				case IDM_SAVE:
					if(dlgSaveOrOpenFile(false, L"Сохранение", L"Снепшот\0*.z80\0Состояние\0*.zx\0Экран\0*.tga\0", L"z80", folder)) {
						settings.curPath = folder.left(folder.find_rev(L'\\') + 1);
						StringZX ext(folder.substr(folder.find_rev(L'.') + 1).lower());
						bool result = false;
						if(ext == L"z80") {
							result = zilog->saveZ80(folder);
						} else if(ext == L"zx") {
							result = zilog->saveStateZX(folder);
						} else if(ext == L"tga") {
							result = gpu->saveScreen(folder);
						}
						if(!result) MessageBox(hWnd, L"Не удалось выполнить операцию!", L"Ошибка", MB_OK);
					}
					break;
				case IDM_RESTORE:
					if(!zilog->loadStateZX(settings.mainDir + L"auto_state.zx")) {
						MessageBox(hWnd, L"Не удалось загрузить состояние машины!", L"Ошибка", MB_OK);
						zilog->signalRESET();
					}
					CheckedMenuModel(hMenu, settings.modelZX);
					break;
				case IDM_TURBO:
					SetTurboMode(true);
					break;
				case IDM_SOUND:
					SetMenuState(&settings.isSound, wmId, true);
					break;
				case IDM_PAUSE:
					SetMenuState(&zilog->isExec, wmId, true, true);
					break;
				case IDM_MONITOR:
					SetMenuState(&settings.isMonitor, wmId, true);
					break;
				case IDM_KEYBOARD:
					SetMenuState(&settings.isKeyboard, wmId, true);
					break;
				case IDM_48K:
					CheckedMenuModel(hMenu, 0);
					break;
				case IDM_128K:
					CheckedMenuModel(hMenu, 1);
					break;
				case IDM_RESET:
					zilog->signalRESET();
					break;
				case IDM_EXIT:
					DestroyWindow(hWnd);
					break;
				default:
					return DefWindowProc(hWnd, message, wParam, lParam);
			}
			break;
		}
		case WM_NOTIFY:
			switch(((LPNMHDR)lParam)->code) {
				case TBN_DROPDOWN: {
					TPMPARAMS tpm;
					RECT rc;

					LPNMTOOLBAR lpnmTB = (LPNMTOOLBAR)lParam;
					SendMessage(lpnmTB->hdr.hwndFrom, TB_GETRECT, (WPARAM)lpnmTB->iItem, (LPARAM)&rc);
					MapWindowPoints(lpnmTB->hdr.hwndFrom, HWND_DESKTOP, (LPPOINT)&rc, 2);

					HMENU hMenuLoaded = LoadMenu(hInst, MAKEINTRESOURCE(IDR_MENU_POPUP_MODEL));
					HMENU hPopupMenu = GetSubMenu(hMenuLoaded, 0);

					CheckedMenuModel(hPopupMenu, settings.modelZX);

					tpm.cbSize = sizeof(TPMPARAMS);
					tpm.rcExclude = rc;

					TrackPopupMenuEx(hPopupMenu, TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_VERTICAL, rc.left, rc.bottom, hWnd, &tpm);
					DestroyMenu(hMenuLoaded);
					break;
				}
			}
			return false;
		case WM_KEYUP: case WM_KEYDOWN: keyb->execute((ssh_b)wParam, message == WM_KEYDOWN); return 0;
		case WM_SIZE: {
			SendMessage(hToolbar, WM_SIZE, wParam, lParam);
			::GetWindowRect(hToolbar, &rectWnd);
			rectWnd.top = rectWnd.bottom - rectWnd.top;
			rectWnd.left = 0;
			rectWnd.right = LOWORD(lParam);
			rectWnd.bottom = HIWORD(lParam);
			break;
		}
		case WM_ERASEBKGND:
			return 1;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow) {

	hInst = hInstance;
	/*
	int hh;
	_wsopen_s(&hh, L"c:\\1.1", _O_RDONLY | _O_BINARY, _SH_DENYRD, _S_IREAD);
	if(hh) {
		ssh_b* a = new ssh_b[1024 * 1024];
		_read(hh, a, 1024 * 1024);
		_close(hh);
		a += 100;
		for(int i = 100; i < 1024 * 1024; i++) {
			if(*a++ != 0) {
				break;
			}
		}
	}
	*/
	UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInst;
	wcex.hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_ZXEMUWND));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_ZXEMUWND);
	wcex.lpszClassName = L"EmuZX";
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	RegisterClassExW(&wcex);

	hWnd = CreateWindowW(L"EmuZX", L"ZxEmuWnd", WS_CLIPCHILDREN | WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 640, 544, nullptr, nullptr, hInst, nullptr);

	if(!hWnd) return false;

	makeToolbar();

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	settings.load(settings.mainDir + L"settings.zx");

	zilog = new CpuZX;
	gpu = new GpuZX;
	brd = new BorderZX;
	snd = new SoundZX;
	keyb = new KeyboardZX;

	zilog->loadStateZX(settings.mainDir + L"auto_state.zx");

	hMenu = GetMenu(hWnd);
	hMenuMRU = GetSubMenu(GetSubMenu(hMenu, 0), 7);

	SetMenuState(&settings.isSound, IDM_SOUND, false);
	SetMenuState(&settings.isMonitor, IDM_MONITOR, false);
	SetMenuState(&settings.isKeyboard, IDM_KEYBOARD, false);
	SetTurboMode(false);
	SetMenuState(&zilog->isExec, IDM_PAUSE, false, true);
	CheckedMenuModel(hMenu, settings.modelZX);
	for(int i = 9 ; i >= 0 ; i--) ModifyMRU(settings.mru[9], false);

	DWORD cpuID;
	HANDLE hCpuThread = CreateThread(nullptr, 0, ProcCPU, nullptr, 1, &cpuID);

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_ZXEMUWND));

    MSG msg;

	while(GetMessage(&msg, nullptr, 0, 0)) {
		if(!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	zilog->saveStateZX(settings.mainDir + L"auto_state.zx");
	settings.save(settings.mainDir + L"settings.zx");
	TerminateThread(hCpuThread, 0);
	return (int)msg.wParam;
}

