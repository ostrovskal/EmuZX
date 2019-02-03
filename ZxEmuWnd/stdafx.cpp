#include "stdafx.h"

zxEmulation theApp;

FILE* hf = nullptr;
int _hf = 0;

thread_local ssh_ws tmpBuf[MAX_PATH];
int lengthTmpBuf;

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow) {
	hInst = hInstance;
	return theApp.run();
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
