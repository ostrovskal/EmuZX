// ZxEmuWnd.cpp: Определяет точку входа для приложения.
//

#include "stdafx.h"
#include "ZxEmuWnd.h"
#include "ZxTimer.h"

long delayCPU = 10;
long delayGPU = 20;
long delayBRD = 160;
long delaySND = 20;

CpuZX* zilog = nullptr;

HWND hWnd;										// хэндл окна
HINSTANCE hInst;                                // текущий экземпляр
RECT rectWnd;

extern "C" {
	void asm_example();
}

DWORD WINAPI ProcCPU(void* params) {
	ZxTimer tm;

	tm.start();
	
	long startCpu = tm.samplePerformanceCounter();
	long startBrd = startCpu;
	long startSnd = startCpu;
	long startGpu = startCpu / tm.millisecondsFrequency;

	while(true) {
		auto current = tm.samplePerformanceCounter();
		auto millis = current / tm.millisecondsFrequency;
		if((current - startCpu) > delayCPU) { zilog->execute(); startCpu = current; }
		if((millis - startGpu) > delayGPU) { zilog->updateGpu(); startGpu = millis; }
		if((current - startBrd) > delayBRD) { zilog->redrawBorder(); startBrd = current; }
		if((current - startSnd) > delaySND) { zilog->playSound(); startSnd = current; }
	}
	tm.stop();
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch(message) {
		case WM_COMMAND: {
			int wmId = LOWORD(wParam);
			// Разобрать выбор в меню:
			switch(wmId) {
				case IDM_SAVE_STATE:
					if(!zilog->saveStateZX(L"c:\\state.zx")) {
						MessageBox(hWnd, L"Не удалось сохранить состояние машины!", L"Ошибка", MB_OK);
					}
					break;
				case IDM_LOAD_STATE:
					if(!zilog->loadStateZX(L"c:\\state.zx")) {
						MessageBox(hWnd, L"Не удалось загрузить состояние машины!", L"Ошибка", MB_OK);
						zilog->signalRESET();
					}
					break;
				case IDM_RESET:
					// reset
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
		case WM_KEYUP: case WM_KEYDOWN: zilog->updateKey(wParam, message == WM_KEYDOWN); break;
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

	asm_example();

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

	zilog = new CpuZX;

	zilog->loadStateZX(L"c:\\state.zx");
	
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
	TerminateThread(hCpuThread, 0);
	return (int)msg.wParam;
}
