#pragma once

struct HEADER_TGA {
	byte	bIdLength;		// 
	byte	bColorMapType;	// ��� �������� ����� ()
	byte	bType;			// ��� ����� ()
	word	wCmIndex;		// ��� �������� � �������
	word	wCmLength;		// ����� �������
	byte	bCmEntrySize;	// ����� ��� �� ������� �������
	word	wXorg;			// 
	word	wYorg;			// 
	word	wWidth;			// ������
	word	wHeight;		// ������
	byte	bBitesPerPixel;	// ��� �� �������
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

