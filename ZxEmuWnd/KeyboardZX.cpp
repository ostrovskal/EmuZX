
#include "stdafx.h"
#include "KeyboardZX.h"
#include "CpuZX.h"

// fe fd fb f7 ef df bf 7f
//	port[0xfefe] = 255;// CAPS_SHIFT Z X C V
//	port[0xfdfe] = 255;// A S D F G
//	port[0xfbfe] = 255;// Q W E R T 
//	port[0xf7fe] = 255;// 1 2 3 4 5
//	port[0xeffe] = 255;// 0 9 8 7 6
//	port[0xdffe] = 255;// P O I U Y
//	port[0xbffe] = 255;// ENTER L K J H
//	port[0x7ffe] = 255;// SPACE SYMBOL_SHIFT M N B
// DEL fefe-254,effe-254,bffe-254

ssh_b remapCtrl[] = {
	VK_CONTROL, 0, 0, 'Z', 0, 1, 'X', 0, 2, 'C', 0, 3, 'V', 0, 4,
	'A', 0, 5, 'S', 0, 6, 'D', 0, 7, 'F', 0, 8, 'G', 0, 9,
	'Q', 0, 10, 'W', 0, 11, 'E', 0, 12, 'R', 0, 13, 'T', 0, 14,
	'1', 0, 15, '2', 0, 16, '3', 0, 17, '4', 0, 18, '5', 0, 19,
	'0', 0, 20, '9', 0, 21, '8', 0, 22, '7', 0, 23, '6', 0, 24,
	'P', 0, 25, 'O', 0, 26, 'I', 0, 27, 'U', 0, 28, 'Y', 0, 29,
	VK_RETURN, 0, 30, 'L', 0, 31, 'K', 0, 32, 'J', 0, 33, 'H', 0, 34,
	VK_SPACE, 0, 35, VK_SHIFT, 0, 36, 'M', 0, 37, 'N', 0, 38, 'B', 0, 39, 255, 255, 255
};

ssh_b remapShift[] = {
	VK_CONTROL, 36, 0, 'Z', 36, 1, 'X', 36, 2, 'C', 36, 3, 'V', 36, 4,
	'A', 36, 5, 'S', 36, 6, 'D', 36, 7, 'F', 36, 8, 'G', 36, 9,
	'Q', 36, 10, 'W', 36, 11, 'E', 36, 12, 'R', 36, 13, 'T', 36, 14,
	'1', 36, 15, '2', 36, 16, '3', 36, 17, '4', 36, 18, '5', 36, 19,
	'P', 36, 25, 'O', 36, 26, 'I', 36, 27, 'U', 36, 28, 'Y', 36, 29,
	VK_RETURN, 36, 30, 'L', 36, 31, 'K', 36, 32, 'J', 36, 33, 'H', 36, 34,
	VK_SPACE, 36, 35, 'M', 36, 37, 'N', 36, 38, 'B', 36, 39,

	254, 254, 254, 254, 254, 254, 254, 254, 254, 254, 254, 254, 254, 254, 254,
	254, 254, 254, 254, 254, 254, 254, 254, 254, 254, 254, 254, 254, 254, 254,

	/* ; */ 186, 36, 1, /* ' */ 222, 36, 25, /* [ */ 219, 36, 8,
	/* ] */ 221, 36, 9, /* , */ 188, 36, 13, /* . */ 190, 36, 14,
	/* / */ 191, 36, 3, /* - */ 189, 36, 20, /* = */ 187, 36, 32,
	/* \ */ 220, 36, 6, '6', 36, 23, '7', 36, 24, '8', 36, 39, '9', 36, 22,
	'0', 36, 21, 189, 36, 20, 255, 255, 255
};

ssh_b remapNone[] = {
	VK_CONTROL, 255, 0, 'Z', 255, 1, 'X', 255, 2, 'C', 255, 3, 'V', 255, 4,
	'A', 255, 5, 'S', 255, 6, 'D', 255, 7, 'F', 255, 8, 'G', 255, 9,
	'Q', 255, 10, 'W', 255, 11, 'E', 255, 12, 'R', 255, 13, 'T', 255, 14,
	'1', 255, 15, '2', 255, 16, '3', 255, 17, '4', 255, 18, '5', 255, 19,
	'0', 255, 20, '9', 255, 21, '8', 255, 22, '7', 255, 23, '6', 255, 24,
	'P', 255, 25, 'O', 255, 26, 'I', 255, 27, 'U', 255, 28, 'Y', 255, 29,
	VK_RETURN, 255, 30, 'L', 255, 31, 'K', 255, 32, 'J', 255, 33, 'H', 255, 34,
	VK_SPACE, 255, 35, 'M', 255, 37, 'N', 255,38, 'B', 255, 39,

	/* BACKSPACE */ 8, 0, 20, /* LEFT */ 37, 0, 19, /* UP */ 38, 0, 23,
	/* RIGHT */ 39, 0, 22, /* DOWN */ 40, 0, 24,

	/* ; */ 186, 36, 26, /* ' */ 222, 36, 23, /* [ */ 219, 36, 29, /* ] */ 221, 36, 28,
	/* , */ 188, 36, 38, /* . */ 190, 36, 37, /* / */ 191, 36, 4, /* - */ 189, 36, 33,
	/* = */ 187, 36, 31, /* \ */ 220, 36, 7, 255, 255, 255
};

ssh_b numPort[] = {0xfe, 0xfd, 0xfb, 0xf7, 0xef, 0xdf, 0xbf, 0x7f};

void KeyboardZX::writePort(ssh_b num, bool pressed) {
	if(num == 255) return;
	ssh_w nport;
	ssh_b msk = (1 << (num % 5));
	for(int i = 0; i < 9; i++) {
		if(i == 8) nport = numPort[num / 5] << 8; else nport = i << 8;
		if(pressed) {
			portsZX[nport | 254] &= ~msk;
		} else {
			portsZX[nport | 254] |= msk;
		}
	}
}

bool KeyboardZX::remapping(ssh_b* map, ssh_b key, bool pressed) {
	int num = 0;
	while(map[num] != 255) {
		if(map[num] == key) {
			if(pnum >= 132) {
				writePort(pkey1, false);
				writePort(pkey2, false);
			}
			pkey1 = map[num + 1];
			pkey2 = map[num + 2];
			if(pkey1 == 255) {
				writePort(0, false);
				writePort(36, false);
			} else writePort(pkey1, pressed);
			writePort(pkey2, pressed);
			pnum = num;
			return true;
		}
		num += 3;
	}
	return false;
}

ssh_b joystick[] = {0, 0, 4, 0, 2, 16, 1, 0, 8, 0};

void KeyboardZX::execute(ssh_b key, bool pressed) {
	if(key >= VK_NUMPAD0 && key <= VK_NUMPAD9) {
		ssh_b bit = joystick[key - VK_NUMPAD0];
		if(pressed) portsZX[31] |= bit; else portsZX[31] &= ~bit;
		return;
	}
	if(key == VK_SHIFT) {
		key = pkey;
		pressed = (::GetKeyState(key) & 128) == 128;
	}
		
	if(key != VK_CONTROL && key != VK_SHIFT) pkey = key;
	
	bool shift = (::GetKeyState(VK_SHIFT) & 128) == 128;
	bool control = (::GetKeyState(VK_CONTROL) & 128) == 128;
	
	if(shift) shift = remapping(remapShift, key, pressed);
	if(!shift) remapping(control ? remapCtrl : remapNone, key, pressed);

}
