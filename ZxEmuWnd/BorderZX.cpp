
#include "stdafx.h"
#include "BorderZX.h"
#include "CpuZX.h"
#include "GpuZX.h"

extern GpuZX* gpu;
extern ssh_d colours[];

void BorderZX::draw() {
	ssh_d* p = &gpu->memory[y * 320];
	ssh_d c = colours[CpuZX::portFE & 7];

	for(int x = 0; x < 320; x++) {
		if(y < 32 || y > 223) *p = c;
		else if(x < 32 || x > 287) *p = c;
		p++;
	}
}

void BorderZX::execute() {
	if(!gpu->memory) return;
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
	CpuZX::STATE &= ~CpuZX::BORDER;
	redraw = false;
	draw();
	y++;
	if(y >= 256) {
		gpu->showScreen();
		y = 0;
		redraw = true;
		start = 0;
	}
}
