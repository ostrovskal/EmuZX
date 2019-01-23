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

HWND hWnd;										// хэндл окна
HINSTANCE hInst;                                // текущий экземпляр
RECT rectWnd;
HMENU hMenu;

DWORD WINAPI ProcCPU(void* params) {
	ZxTimer tm;

	tm.start();
	
	DWORD startCpu = tm.samplePerformanceCounter();
	DWORD startBrd = startCpu;
	DWORD startGpu = startCpu / tm.millisecondsFrequency;

	while(true) {
		DWORD current = tm.samplePerformanceCounter();
		DWORD millis = current / tm.millisecondsFrequency;
		if((current - startCpu) > settings.delayCPU) { zilog->execute(); startCpu = current; }
		if((millis - startGpu) > settings.delayGPU) { gpu->execute(); startGpu = millis; }
		if((current - startBrd) > (settings.delayCPU * 16)) { brd->execute(); startBrd = current; }
		if(settings.isSound) snd->execute(current);
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

void CheckedMenuModel(int rom) {
	static int mid[] = {IDM_48K, IDM_128K};
	for(int nid : mid) CheckMenuItem(hMenu, nid, 0);
	CheckMenuItem(hMenu, mid[rom], MF_CHECKED);
	if(settings.modelZX != rom) {
		settings.modelZX = rom;
		zilog->signalRESET();
	}
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch(message) {
		case WM_COMMAND: {
			int wmId = LOWORD(wParam);
			StringZX folder(settings.curPath);
			// Разобрать выбор в меню:
			switch(wmId) {
				case IDM_OPEN:
					if(dlgSaveOrOpenFile(true, L"Открыть", L"Снепшот\0*.z80\0Состояние\0*.zx\0", L"z80", folder)) {
						settings.curPath = folder.left(folder.find_rev(L'\\') + 1);
						StringZX ext(folder.substr(folder.find_rev(L'.') + 1).lower());
						bool result = false;
						if(ext == L"z80") {
							result = zilog->loadZ80(folder);
						} else if(ext == L"zx") {
							result = zilog->loadStateZX(folder);
						}
						if(!result) MessageBox(hWnd, L"Не удалось выполнить операцию!", L"Ошибка", MB_OK);
					}
					break;
				case IDM_SAVE:
					if(dlgSaveOrOpenFile(false, L"Сохранение", L"Снепшот\0*.z80\0Состояние\0*.zx\0", L"z80", folder)) {
						settings.curPath = folder.left(folder.find_rev(L'\\') + 1);
						StringZX ext(folder.substr(folder.find_rev(L'.') + 1).lower());
						bool result = false;
						if(ext == L"z80") {
							result = zilog->saveZ80(folder);
						} else if(ext == L"zx") {
							result = zilog->saveStateZX(folder);
						}
						if(!result) MessageBox(hWnd, L"Не удалось выполнить операцию!", L"Ошибка", MB_OK);
					}
					break;
				case IDM_SAVE_SCREEN:
					if(dlgSaveOrOpenFile(false, L"Сохранение", L"TGA\0*.tga\0", L"tga", folder)) {
						settings.curPath = folder.left(folder.find_rev(L'\\') + 1);
						if(!gpu->saveScreen(folder))
							MessageBox(hWnd, L"Не удалось сохранить экран!", L"Ошибка", MB_OK);
					}
					break;
				case IDM_RESTORE:
					if(!zilog->loadStateZX(settings.mainDir + L"auto_state.zx")) {
						MessageBox(hWnd, L"Не удалось загрузить состояние машины!", L"Ошибка", MB_OK);
						zilog->signalRESET();
					}
					break;
				case IDM_SOUND:
					settings.isSound = !settings.isSound;
					CheckMenuItem(hMenu, wmId, settings.isSound << 3);
					break;
				case IDM_PAUSE:
					zilog->isExec = !zilog->isExec;
					CheckMenuItem(hMenu, wmId, (!zilog->isExec) << 3);
					break;
				case IDM_MONITOR:
					settings.isMonitor = !settings.isMonitor;
					CheckMenuItem(hMenu, wmId, settings.isMonitor << 3);
					break;
				case IDM_KEYBOARD:
					settings.isKeyboard = !settings.isKeyboard;
					CheckMenuItem(hMenu, wmId, settings.isKeyboard << 3);
					break;
				case IDM_48K:
					CheckedMenuModel(0);
					break;
				case IDM_128K:
					CheckedMenuModel(1);
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
		case WM_KEYUP: case WM_KEYDOWN: keyb->execute((ssh_b)wParam, message == WM_KEYDOWN); return 0;
		case WM_SIZE:
			::GetClientRect(hWnd, &rectWnd);
			return 0;
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
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ZXEMUWND));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_ZXEMUWND);
	wcex.lpszClassName = L"EmuZX";
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	RegisterClassExW(&wcex);

	hWnd = CreateWindowW(L"EmuZX", L"ZxEmuWnd", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 640, 544, nullptr, nullptr, hInstance, nullptr);

	if(!hWnd) return FALSE;

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	settings.load(settings.mainDir + L"settings.opt");

	hMenu = GetMenu(hWnd);
	CheckMenuItem(hMenu, IDM_SOUND, settings.isSound << 3);
	CheckMenuItem(hMenu, IDM_MONITOR, settings.isMonitor << 3);
	CheckMenuItem(hMenu, IDM_KEYBOARD, settings.isKeyboard << 3);
	CheckedMenuModel(settings.modelZX);

	zilog = new CpuZX;
	gpu = new GpuZX;
	brd = new BorderZX;
	snd = new SoundZX;
	keyb = new KeyboardZX;

	zilog->loadStateZX(settings.mainDir + L"auto_state.zx");	

	DWORD cpuID;
	HANDLE hCpuThread = CreateThread(nullptr, 0, ProcCPU, nullptr, 1, &cpuID);

	hInst = hInstance;
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

