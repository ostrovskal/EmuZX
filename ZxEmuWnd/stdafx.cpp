#include "stdafx.h"

zxFrame* theApp = nullptr;

ssh_d HEX = 0;

ssh_b TMP_BUF[131072];
//ssh_ws* tmpBuf((ssh_ws*)TMP_BUF);

THREAD_STATE thState;

extern CRITICAL_SECTION cs;

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow) {
	hInst = hInstance;
	zxString str(5, RFMT_OFFS_P + 1, true);
	zxFrame frame;
	theApp = &frame;
	return frame.run();
}

bool msgPump() {
	MSG* msg = &thState.msg;
	if (!::GetMessage(msg, NULL, NULL, NULL)) {
		PostQuitMessage(0);
		return false;
	}
	auto wnd = zxWnd::fromHWND(msg->hwnd);
	bool is = false;
	if (wnd) {
		is = wnd->translateAccelerator(msg);
		if (!is) is = wnd->dialogMessage(msg);
	}
	if (!is) {
		TranslateMessage(msg);
		DispatchMessage(msg);
	}
	return true;
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

/*
ssh_cws fromNum(int num, ssh_cws fmt) {
	swprintf_s(tmpBuf, MAX_PATH, fmt, num);
	return tmpBuf;
}
*/

// вернуть адрес памяти
ssh_b* get_mem(ssh_w address) {
	if(address < 16384) return ((*_TSTATE & ZX_TR_DOS) ? &TR_DOS[address] : &pageROM[address]);
	else if(address < 32768) return &PAGE_RAM[5][address - 16384];
	else if(address < 49152) return &PAGE_RAM[2][address - 32768];
	return &pageRAM[address - 49152];
}

// распаковка блока памяти (с учетом или без завершающей сигнатуры)
bool unpackBlock(ssh_b* src, ssh_b* dst, ssh_b* dstE, int sz, bool packed, bool sign) {
	if(packed) {
		memset(dst, 0, sz);
		while(sz > 0 && dst < dstE) {
			sz--;
			ssh_b b = *src++, c = 1;
			if (b == 0xed && *src == 0xed) {
				src++;
				c = *src++;
				b = *src++;
				sz -= 3;
			}
			memset(dst, b, c);
			dst += c;
		}
		if(sign) return (sz == 4 && (*(ssh_d*)src == 0x00eded00));
		return (sz == 0);
	}
	memcpy(dst, src, sz);
	return true;
}

static void packSegment(ssh_b** dst, int count, ssh_b block) {
	auto dest = *dst;
	if (count == 1) {
		*dest++ = block;
	} else if (count < 5 && block != 0xed) {
		memset(*dst, block, count);
		dest += count;
	} else {
		if (*(dest - 1) == 0xed) {
			*dest++ = block;
			count--;
		}
		*dest++ = 0xed; *dest++ = 0xed;
		*dest++ = count; *dest++ = block;
	}
	*dst = dest;
}

// распаковка блока памяти (с учетом или без завершающей сигнатуры)
ssh_b* packBlock(ssh_b* src, ssh_b* srcE, ssh_b* blk, bool sign, ssh_d& newSize) {
	ssh_b* dst = blk, block;
	int count = 0;
	while(src < srcE) {
		block = *src++;
		count++;
		if (count == 255 || block != *src) {
			packSegment(&dst, count, block);
			count = 0;
		}
	}
	if (count) packSegment(&dst, count, block);
	if(sign) {
		*(ssh_d*)dst = 0x00eded00;
		dst += 4;
	}
	newSize = (ssh_d)(dst - blk);
	return blk;
}

ssh_w modifyTSTATE(int adding, int remove) {
	EnterCriticalSection(&cs);
	(*_TSTATE) &= ~remove;
	(*_TSTATE) |= adding;
	LeaveCriticalSection(&cs);
	return (*_TSTATE);
}

bool dlgSaveOrOpenFile(bool bOpen, ssh_cws title, ssh_cws filter, ssh_cws defExt, zxString& folder) {
	bool result;
	static ssh_ws flt[MAX_PATH];

	int flags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY;

	OPENFILENAME ofn;
	memset(&ofn, 0, sizeof(OPENFILENAME));

	if(bOpen) flags |= OFN_FILEMUSTEXIST;

	auto files = new ssh_ws[MAX_PATH];
	memset(files, 0, MAX_PATH);

	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = theApp->getHWND();

	ofn.lpstrDefExt = defExt;
	ofn.lpstrFilter = filter;
	ofn.lpstrInitialDir = folder;
	ofn.lpstrTitle = title;
	ofn.lpstrFile = files;
	ofn.lpstrFileTitle = nullptr;
	ofn.nFilterIndex = 1;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = flags;

	result = bOpen ? ::GetOpenFileName(&ofn) : ::GetSaveFileName(&ofn);
	if(result) folder = ofn.lpstrFile;
	SAFE_DELETE(files);
	return result;
}
