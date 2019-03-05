
#include "stdafx.h"
#include "zxGPU.h"
#include "zxSettings.h"

#pragma pack(push, 1)

struct HEADER_TGA {
	ssh_b	bIdLength;		// 
	ssh_b	bColorMapType;	// тип цветовой карты ()
	ssh_b	bType;			// тип файла ()
	ssh_w	wCmIndex;		// тип индексов в палитре
	ssh_w	wCmLength;		// длина палитры
	ssh_b	bCmEntrySize;	// число бит на элемент палитры
	ssh_w	wXorg;			// 
	ssh_w	wYorg;			// 
	ssh_w	wWidth;			// ширина
	ssh_w	wHeight;		// высота
	ssh_b	bBitesPerPixel;	// бит на пиксель
	ssh_b	bFlags;			// 
};

#pragma pack(pop)

zxGPU::zxGPU() {
	blink = blinkMsk = blinkShift = 0;
	sizeBorder = -1;
	rect.left = rect.top = rect.right = rect.bottom = 0;
	memoryPrimary = nullptr;
	memoryBuffer = nullptr;
	memset(colours, 0, sizeof(colours));
	bmi = BITMAPINFO{ { sizeof(BITMAPINFOHEADER), 0, 0, 1, USHORT(32), BI_RGB, 0, 0, 0, 0, 0 }, {0, 0, 0, 0} };
}

void zxGPU::updateData() {
	// параметры
	int border = theApp->getOpt(OPT_SIZE_BORDER)->dval * 8;
	if (border != sizeBorder) {
		delete[] memoryBuffer;
		sizeBorder = border;
		rect.left = 256 + sizeBorder * 2;
		rect.top = 192 + sizeBorder * 2;
		memoryBuffer = new ssh_d[rect.left * rect.top];
	}
	// цвета
	for(int i = 0; i < 16; i++) colours[i] = theApp->getOpt(OPT_COLOR_BLACK + i)->dval;
	// скорость мерцания
	blinkShift = theApp->getOpt(OPT_PERIOD_BLINK)->dval;
	blinkMsk = (1 << (blinkShift + 1)) - 1;
}

void zxGPU::updateFrame() {
	theApp->bus.updateFrame(memoryBuffer, colours, sizeBorder, ((blink & blinkMsk) >> blinkShift) == 0);
	LPRECT r = &theApp->wndRect;
	int width = r->right - r->left;
	int height = r->bottom - r->top;
	if(width != rect.right || height != rect.bottom) {
		delete[] memoryPrimary;
		rect.right = width;
		rect.bottom = height;
		bmi.bmiHeader.biWidth = width;
		bmi.bmiHeader.biHeight = -height;
		memoryPrimary = new ssh_d[width * height];
	}
	asm_ssh_post_process(theApp->getOpt(OPT_FILTER)->dval, &rect, memoryBuffer, memoryPrimary);
	HDC hdc;
	if((hdc = ::GetDC(theApp->getHWND()))) {
		StretchDIBits(hdc, r->left, r->top, width, height, 0, 0, width, height, memoryPrimary, &bmi, DIB_RGB_COLORS, SRCCOPY);
		::DeleteObject(hdc);
	}
	blink++;
}

bool zxGPU::saveScreen(ssh_cws path) {
	zxFile f(path, false);
	HEADER_TGA head{0, 0, 2, 0, 0, 0, 0, 0, (ssh_w)rect.right, (ssh_w)rect.bottom, 32, 40};
	if(!f.write(&head, sizeof(HEADER_TGA))) return false;
	return f.write(memoryPrimary, rect.right * rect.bottom * 4);
}
