
#include "stdafx.h"
#include "BorderZX.h"
#include "CpuZX.h"
#include "GpuZX.h"

extern ssh_d colours[];

void BorderZX::draw() {
	ssh_d* p = &gpu->memory[y * 320];
	ssh_b col = (*_PORT_FE) & 7;
	ssh_d c = colours[col];

	for(int x = 0; x < 320; x++) {
		if(y < 32 || y > 223) *p = c;
		else if(x < 32 || x > 287) *p = c;
		p++;
	}
}

void BorderZX::execute() {
	if(!gpu->memory) return;
	if((_TSTATE & ZX_BORDER) == 0 && y == 0) {
		if(redraw) {
			start++;
			if(start > 10) {
				for(y = 0; y < 256; y++) draw();
				y = 0;
				redraw = false;
				theApp.gpu->showScreen();
			}
		}
		return;
	}
	_TSTATE &= ~ZX_BORDER;
	redraw = false;
	draw();
	y++;
	if(y >= 256) {
		theApp.gpu->showScreen();
		y = 0;
		redraw = true;
		start = 0;
	}
}
