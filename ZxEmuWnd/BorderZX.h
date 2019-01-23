#pragma once

class BorderZX {
public:
	BorderZX() : y(0), redraw(false), start(0) {}
	virtual ~BorderZX() {}
	void execute();
protected:
	void draw();
	int y;
	bool redraw;
	long start;
};