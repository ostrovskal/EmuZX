#include "stdafx.h"

zxFrame* theApp = nullptr;

FILE* hf = nullptr;
int _hf = 0;

thread_local ssh_ws tmpBuf[MAX_PATH];
int lengthTmpBuf;
THREAD_STATE thState;

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow) {
	hInst = hInstance;

	zxFrame frame;
	theApp = &frame;
	return frame.run();
}

bool msgPump() {
	MSG* msg = &thState.msg;
	if(!::GetMessage(msg, NULL, NULL, NULL)) {
		PostQuitMessage(0);
		return false;
	}
	auto wnd = zxWnd::fromHWND(msg->hwnd);
	bool is = false;
	if(wnd) {
		is = wnd->translateAccelerator(msg);
		if(!is) is = wnd->dialogMessage(msg);
	}
	if(!is) {
		TranslateMessage(msg);
		DispatchMessage(msg);
	}
	return true;
	/*
	void
	win32ui_process_messages( int process_queue_once )
	{
	MSG msg;
	int i, processMsg;
	HWND hModelessDlgs[] = { fuse_hPFWnd, fuse_hDBGWnd, fuse_hABOWnd };

	while( 1 ) {
	while( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) ) {
	processMsg = TRUE;

	for(i = 0; processMsg && i < ARRAY_SIZE(hModelessDlgs); i++) {
		if(IsDialogMessage(hModelessDlgs[i], &msg)) processMsg = FALSE;
	}

	if(processMsg) {
		if(!TranslateAccelerator(fuse_hWnd, hAccels, &msg)) {
			if((LOWORD(msg.message) == WM_QUIT) ||
				(LOWORD(msg.message) == WM_USER_EXIT_PROCESS_MESSAGES))
				return;
			// FIXME: set exit flag somewhere 
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
}
if(process_queue_once) return;

// If we miss WM_USER_EXIT_PROCESS_MESSAGES, the window procedure will kindly notify us
if(exit_process_messages) {
	exit_process_messages--;
	return;
}

WaitMessage();
  } 	
  */
}

// преобразовать значение в ближайшую степень двойки
ssh_u ssh_pow2(ssh_u val, bool nearest) {
	ssh_d idx;
	_BitScanReverse64(&idx, val);
	ssh_u _val((1ULL << idx));
	return ((_val == val || nearest) ? _val : _val << 1ULL);
}

void* ssh_memset(void* ptr, int set, ssh_u count) {
	if(ptr) ptr = (ssh_b*)(memset((void*)ptr, set, count)) + count;
	return ptr;
}

void* ssh_memzero(void* ptr, ssh_u count) {
	return ssh_memset(ptr, 0, count);
}

void* ssh_memcpy(void* dst, const void* src, ssh_u count) {
	if(dst && src) dst = (ssh_b*)(memcpy((void*)dst, src, count)) + count;
	return dst;
}

ssh_cws fromNum(int num, ssh_cws fmt) {
	lengthTmpBuf = swprintf_s(tmpBuf, MAX_PATH, fmt, num);
	return tmpBuf;
}

void writePort(ssh_b a1, ssh_b a2, ssh_b v) {

}

ssh_b readPort(ssh_b a1, ssh_b a2) {
	return 0;
}

// вернуть адрес памяти
ssh_b* get_mem(ssh_w address) {
	if(address < 16384) return &pageROM[address];
	else if(address < 32768) return &PAGE_RAM[5][address - 16384];
	else if(address < 49152) return &PAGE_RAM[2][address - 32768];
	return &pageRAM[address - 49152];
}
