#pragma once

class GpuZX {
public:
	GpuZX();
	virtual ~GpuZX();
	void execute();
	bool saveScreen(ssh_cws path);
	void write(ssh_b* address, ssh_b val);
	void showScreen();
protected:
	void drawLine(ssh_d* addr, ssh_b val);
	void decodeColor(ssh_b color);
	void makeCanvas();
	ssh_d* memory;
	ssh_b invert;
	ssh_d ink;
	ssh_d paper;
	HBITMAP hbmpMem;
	HDC hdcMem;
};

