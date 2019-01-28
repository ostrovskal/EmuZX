#pragma once

class GpuZX;
class BorderZX {
public:
	BorderZX() : y(0), redraw(false), start(0) { gpu = theApp.gpu; }
	virtual ~BorderZX() {}
	void execute();
protected:
	GpuZX * gpu;
	void draw();
	int y;
	bool redraw;
	long start;
};