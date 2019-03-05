#pragma once

class zxGPU {
public:
	zxGPU();
	virtual ~zxGPU() { delete[] memoryPrimary; }
	void updateFrame();
	bool saveScreen(ssh_cws path);
	void updateData();
protected:
	RECT rect;
	int sizeBorder;
	ssh_d* memoryPrimary, *memoryBuffer;
	ssh_d blink, blinkMsk, blinkShift;
	BITMAPINFO bmi;
	ssh_d colours[16];
};

