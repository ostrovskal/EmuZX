
#include "stdafx.h"
#include "BorderZX.h"
#include "CpuZX.h"

int BorderZX::frames = 0;

extern dword colours[];

void BorderZX::draw() {
	dword* p = &gpu->memory[y * 320];
	dword c = colours[CpuZX::border];

	for(int x = 0; x < 320; x++) {
		if(y < 32 || y > 223) *p = c;
		else if(x < 32 || x > 287) *p = c;
		p++;
	}
}

void BorderZX::execute() {
	if(!gpu->memory) return;
	frames++;
	if((CpuZX::STATE & CpuZX::BORDER) == 0 && y == 0) {
		if(redraw) {
			start++;
			if(start > 10) {
				for(y = 0; y < 256; y++) draw();
				y = 0;
				redraw = false;
				gpu->showScreen();
			}
		}
		return;
	}
	redraw = false;
	draw();
	y++;
	if(y >= 256) {
		gpu->showScreen();
		CpuZX::STATE &= ~CpuZX::BORDER;
		y = 0;
		redraw = true;
		start = 0;
	}
}
