#pragma once

class GpuZX;
class BorderZX {
public:
	BorderZX() : y(0), redraw(false), start(0) {}
	virtual ~BorderZX() {}

	void execute();

	GpuZX* gpu;

	static int frames; 
protected:
	void draw();
	int y;
	bool redraw;
	long start;
};