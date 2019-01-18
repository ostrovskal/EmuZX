#pragma once

struct HEADER_TGA {
	byte	bIdLength;		// 
	byte	bColorMapType;	// тип цветовой карты ()
	byte	bType;			// тип файла ()
	word	wCmIndex;		// тип индексов в палитре
	word	wCmLength;		// длина палитры
	byte	bCmEntrySize;	// число бит на элемент палитры
	word	wXorg;			// 
	word	wYorg;			// 
	word	wWidth;			// ширина
	word	wHeight;		// высота
	byte	bBitesPerPixel;	// бит на пиксель
	byte	bFlags;			// 
};

class GpuZX {
public:
	GpuZX();
	virtual ~GpuZX();
	void execute();
	void saveBitmap(const wchar_t* path);
	void write(byte* address, byte val);
	void showScreen();
	dword* memory;
	static int frames;
protected:
	void drawLine(dword* addr, byte val);
	void decodeColor(byte color);
	void makeCanvas();
	byte invert;
	dword ink;
	dword paper;
	HBITMAP hbmpMem;
	HDC hdcMem;
};

