#pragma once

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

class GpuZX {
public:
	GpuZX();
	virtual ~GpuZX();
	void execute();
	bool saveScreen(ssh_cws path);
	void write(ssh_b* address, ssh_b val);
	void showScreen();
	ssh_d* memory;
protected:
	void drawLine(ssh_d* addr, ssh_b val);
	void decodeColor(ssh_b color);
	void makeCanvas();
	ssh_b invert;
	ssh_d ink;
	ssh_d paper;
	HBITMAP hbmpMem;
	HDC hdcMem;
};

