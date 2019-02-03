
#include "stdafx.h"
#include "GpuZX.h"
#include "CpuZX.h"
#include "SettingsZX.h"
#include "zxKeyboard.h"

ssh_d colours[] = {	0xff000000, 0xff2030c0, 0xffc04010, 0xffc040c0, 0xff40b010, 0xff50c0b0, 0xffe0c010, 0xffc0c0c0,
					0xff000000, 0xff3040ff, 0xffff4030, 0xffff70f0, 0xff50e010, 0xff50e0ff, 0xffffe850, 0xffffffff};

ssh_d scan_offs[] = {
	0, 256, 512, 768, 1024, 1280, 1536, 1792,
	32, 288, 544, 800, 1056, 1312, 1568, 1824,
	64, 320, 576, 832, 1088, 1344, 1600, 1856,
	96, 352, 608, 864, 1120, 1376, 1632, 1888,
	128, 384, 640, 896, 1152, 1408, 1664, 1920,
	160, 416, 672, 928, 1184, 1440, 1696, 1952,
	192, 448, 704, 960, 1216, 1472, 1728, 1984,
	224, 480, 736, 992, 1248, 1504, 1760, 2016
};
GpuZX::GpuZX() {
	invert = 0;
	hbmpMem = nullptr;
	hdcMem = nullptr;
	memory = nullptr;
}

GpuZX::~GpuZX() {
	::DeleteObject(hbmpMem);
	::DeleteObject(hdcMem);
}

void GpuZX::makeCanvas() {
	BITMAPINFO bmi;

	memset(&bmi.bmiHeader, 0, sizeof(bmi.bmiHeader));
	bmi.bmiHeader.biSize = sizeof(bmi.bmiHeader);
	bmi.bmiHeader.biWidth = 288;
	bmi.bmiHeader.biHeight = 224;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = USHORT(32);
	bmi.bmiHeader.biCompression = BI_RGB;

	hbmpMem = ::CreateDIBSection(NULL, &bmi, DIB_RGB_COLORS, (void**)&memory, NULL, 0);
	hdcMem = ::CreateCompatibleDC(NULL);
	ssh_d* mem = memory;
	for(int i = 0; i < 224 * 288; i++) *mem++ = 0xffc0c0c0;
}

void GpuZX::showScreen() {
	HDC hdc;
	if((hdc = ::GetDC(theApp.getHWND()))) {
		HGDIOBJ h = SelectObject(hdcMem, hbmpMem);
		LPRECT r = &theApp.wndRect;
		StretchBlt(hdc, r->left, r->top, r->right - r->left, r->bottom - r->top, hdcMem, 0, 0, 288, 224, SRCCOPY);
		SelectObject(hdcMem, h);
		::DeleteObject(hdc);
		StringZX::fmt(L"SshZX (%s : %d) %s [%s]", ((_TSTATE & ZX_EXEC) ? L"execute" : L"pause"), _PC,
					  nameROMs[theApp.getOpt(OPT_MEM_MODEL)->dval], theApp.opts.nameLoadProg.str());
		SetWindowText(theApp.getHWND(), (LPCWSTR)tmpBuf);
	}
	invert++;
}

void GpuZX::execute() {

	if(!hbmpMem) makeCanvas();
	
	int y = *_SCAN;
	auto dest = &memory[y * 288];
	ssh_b col = (*_PORT_FE) & 7;
	ssh_d c = colours[col];

	for(int x = 0; x < 288; x++) {
		if(y < 16 || y > 207) *(dest + x) = c;
		else if(x < 16 || x > 271) *(dest + x) = c;
	}
	if(y > 15 && y < 208) {
		// ���������� ���� ����� ������
		int yy = 191 - (y - 16);
		int y_bank = (yy / 64) * 2048;
		int offs = scan_offs[yy & 63] + y_bank;

		dest += 16;
		auto src_cols = ((yy >> 3) * 32) + (memScreen + 6144);
		auto src = memScreen + offs;

		for(int x = 0; x < 32; x++) {
			decodeColor(*(src_cols + x));
			drawLine(dest, *(src + x));
			dest += 8;
		}
	}
	y++;
	if(y > 223) y = 0;
	*_SCAN = y;
	if(!y) {
		auto filter = theApp.getOpt(OPT_PP)->dval;
		if(filter > 0) {
			dest = &memory[0];
			int x = 0, y = 1;
			while(y++ < 223) {
				while(x++ < 287) {
					*dest++ = (filter == 1 ? asm_ssh_mixed(dest, dest + 288) : asm_ssh_bilinear(x, y, 288, dest));
				}
				//dest += 32;
				x = 0;
			}
		}
	}
}

void GpuZX::decodeColor(ssh_b color) {
	bool inv = ((color & 128) ? ((invert & 15) >> 3) == 0 : false);
	ssh_d ink1 = colours[(color & 7) | (color & 64) >> 3];
	ssh_d pap1 = colours[(color & 120) >> 3];
	ink = (inv ? pap1 : ink1);
	paper = (inv ? ink1 : pap1);
}

void GpuZX::write(ssh_b* address, ssh_b val) {
	ssh_w offs = (ssh_w)(address - memScreen);
	int x, y;
	ssh_d* addr = &memory[32 * 320 + 32];
	if(offs >= 6144) {
		// ��������
		offs -= 6144;
		x = offs & 31;
		y = (offs / 32);
		decodeColor(val);
		ssh_b* screen = &memZX[16384 + ((y / 8) * 2048 + ((y & 7) * 32)) + x];
		addr += ((24 - y) * 8 * 320 + x * 8 - 320);
		for(int n = 0; n < 8; n++) {
			drawLine(addr, *screen);
			screen += 256;
			addr -= 320;
		}
	} else {
		// �������
		int bank = (offs / 2048) * 64;
		offs &= 2047;

		int h = offs / 256;
		int l = offs & 255;

		x = offs & 31;
		y = 191 - (bank + (h + (l / 32) * 8));
		addr += (y * 320 + x * 8);
		decodeColor(memZX[22528 + (y / 8) * 32 + x]);
		drawLine(addr, val);
	}
}

void GpuZX::drawLine(ssh_d* addr, ssh_b val) {
	for(int bit = 128; bit > 0; bit >>= 1) {
		*addr++ = ((val & bit) ? ink : paper);
	}
}

bool GpuZX::saveScreen(ssh_cws path) {
	bool result = true;

	theApp.pauseCPU(true, 0);

	try {
		_wsopen_s(&_hf, path, _O_CREAT | _O_TRUNC | _O_WRONLY | _O_BINARY, _SH_DENYWR, _S_IWRITE);
		if(_hf != -1) {
			HEADER_TGA head;

			head.bIdLength = 0;
			head.bColorMapType = 0;
			head.bType = 2;

			head.wCmIndex = 0;
			head.wCmLength = 0;
			head.bCmEntrySize = 0;

			head.wXorg = 0;
			head.wYorg = 0;
			head.wWidth = 320;
			head.wHeight = 256;

			head.bBitesPerPixel = 32;
			head.bFlags = 0x28;

			if(_write(_hf, &head, sizeof(HEADER_TGA)) != sizeof(HEADER_TGA)) throw(0);
			ssh_d* ptr = (memory + 320 * 255);
			for(int i = 0; i < 256; i++) {
				if(_write(_hf, ptr, 1280) != 1280) throw(0);
				ptr -= 320;
			}
		}
	} catch(...) { result = false; }

	SAFE_CLOSE1(_hf);

	theApp.pauseCPU(false, 0);

	return result;
}
