#pragma once

#define SIZE_BORDER			32
#define WIDTH_SCREEN		32 * 8
#define HEIGHT_SCREEN		192

class GpuZX {
public:
	GpuZX();
	virtual ~GpuZX();
	void execute();
	bool saveScreen(ssh_cws path);
	void showScreen();
protected:
	void drawLine(ssh_d* addr, ssh_b val);
	void decodeColor(ssh_b color);
	void makeCanvas();
	ssh_d* memBuffer(bool back);
	ssh_d* memoryPrimary, *memoryBack;
	ssh_b invert;
	ssh_d ink;
	ssh_d paper;
	HBITMAP hbmpMemPrimary, hbmpMemBack;
	HDC hdcMemPrimary, hdcMemBack;
};

