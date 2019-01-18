
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

word numPort[] = {0xfefe, 0xfdfe, 0xfbfe, 0xf7fe, 0xeffe, 0xdffe, 0xbffe, 0x7ffe};

byte remapCtrl[] = {
	VK_CONTROL, -1, 0, 'Z', -1, 1, 'X', -1, 2, 'C', -1, 3, 'V', -1, 4,
	'A', -1, 5, 'S', -1, 6, 'D', -1, 7, 'F', -1, 8, 'G', -1, 9,
	'Q', -1, 10, 'W', -1, 11, 'E', -1, 12, 'R', -1, 13, 'T', -1, 14,
	'1', -1, 15, '2', -1, 16, '3', -1, 17, '4', -1, 18, '5', -1, 19,
	'0', -1, 20, '9', -1, 21, '8', -1, 22, '7', -1, 23, '6', -1, 24,
	'P', -1, 25, 'O', -1, 26, 'I', -1, 27, 'U', -1, 28, 'Y', -1, 29,
	VK_RETURN, -1, 30, 'L', -1, 31, 'K', -1, 32, 'J', -1, 33, 'H', -1, 34,
	VK_SPACE, -1, 35, VK_SHIFT, -1, 36, 'M', -1, 37, 'N', -1,38, 'B', -1, 39,
	-1, -1, -1
};

byte keys2[] = {
	/* BACKSPACE */ 8, 0, 20, 20, /* LEFT */ 37, 0, 19, 19, /* UP */ 38, 0, 23, 23,
	/* RIGHT */ 39, 0, 22, 22, /* DOWN */ 40, 0, 24, 24,
	/* ; */ 186, 36, 26, 1,
	/* ' */ 222, 36, 23, 25, /* [ */ 219, 36, 29, 8, /* ] */ 221, 36, 28, 9,
	/* , */ 188, 36, 38, 13, /* . */ 190, 36, 37, 14, /* / */ 191, 36, 4, 3,
	/* - */ 189, 36, 33, 33, /* = */ 187, 36, 31, 32, /* \ */ 220, 36, 7, 6,

};

byte remapShift[] = {
	/* ; */ 186, 36, 1,
	/* ' */ 222, 36, 25, /* [ */ 219, 36, 8, /* ] */ 221, 36, 9,
	/* , */ 188, 36, 13, /* . */ 190, 36, 14, /* / */ 191, 36, 3,
	/* - */ 189, 36, 33, /* = */ 187, 36, 32, /* \ */ 220, 36, 6,
	'7', 36, 22, -1, -1, -1
};
//	VK_CONTROL, -1, -1, 'Z', -1, 1, 'X', -1, 2, 'C', -1, 3, 'V', -1, 4,
//	'A', -1, 5, 'S', -1, 6, 'D', -1, 7, 'F', -1, 8, 'G', -1, 9,
//	'Q', -1, 10, 'W', -1, 11, 'E', -1, 12, 'R', -1, 13, 'T', -1, 14,
//	'1', -1, 15, '2', -1, 16, '3', -1, 17, '4', -1, 18, '5', -1, 19,
//	'0', -1, 20, '9', -1, 21, '8', -1, 22, '7', -1, 23, '6', -1, 24,
//	'P', -1, 25, 'O', -1, 26, 'I', -1, 27, 'U', -1, 28, 'Y', -1, 29,
//	VK_RETURN, -1, 30, 'L', -1, 31, 'K', -1, 32, 'J', -1, 33, 'H', -1, 34,
//	VK_SPACE, -1, 35, VK_SHIFT, -1, -1, 'M', -1, 37, 'N', -1,38, 'B', -1, 39,
//	-1, -1, -1
//};

byte remapNone[] = {
	VK_CONTROL, -1, -1, 'Z', -1, 1, 'X', -1, 2, 'C', -1, 3, 'V', -1, 4,
	'A', -1, 5, 'S', -1, 6, 'D', -1, 7, 'F', -1, 8, 'G', -1, 9,
	'Q', -1, 10, 'W', -1, 11, 'E', -1, 12, 'R', -1, 13, 'T', -1, 14,
	'1', -1, 15, '2', -1, 16, '3', -1, 17, '4', -1, 18, '5', -1, 19,
	'0', -1, 20, '9', -1, 21, '8', -1, 22, '7', -1, 23, '6', -1, 24,
	'P', -1, 25, 'O', -1, 26, 'I', -1, 27, 'U', -1, 28, 'Y', -1, 29,
	VK_RETURN, -1, 30, 'L', -1, 31, 'K', -1, 32, 'J', -1, 33, 'H', -1, 34,
	VK_SPACE, -1, 35, VK_SHIFT, -1, -1, 'M', -1, 37, 'N', -1,38, 'B', -1, 39,
	/* ; */ 186, 36, 26, /* ' */ 222, 36, 23, /* [ */ 219, 36, 29, /* ] */ 221, 36, 28,
	/* , */ 188, 36, 38, /* . */ 190, 36, 37, /* / */ 191, 36, 4, /* - */ 189, 36, 33,
	/* = */ 187, 36, 31, /* \ */ 220, 36, 7, /* BACKSPACE */ 8, 0, 20, /* LEFT */ 37, 0, 19, 
	/* UP */ 38, 0, 23, /* RIGHT */ 39, 0, 22, /* DOWN */ 40, 0, 24, -1, -1, -1
};

void KeyboardZX::setKey(int num, bool pressed) {
	if(num == -1) return;
	word nport = numPort[num / 5];
	byte msk = (1 << (num % 5));
	if(pressed) {
		CpuZX::port[nport] &= ~msk;
	} else {
		CpuZX::port[nport] |= msk;
	}
}

void KeyboardZX::remapping(byte* map, int key, bool pressed) {
	while(*map != -1) {
		if(*map == key) {
			setKey(spec, false);
			setKey(prevKey, false);
			spec = map[1];
			prevKey = map[2];
			setKey(spec, pressed);
			setKey(prevKey, pressed);
			break;
		}
		map += 3;
	}
}

void KeyboardZX::execute(int key, bool pressed) {
	bool shift = ::GetKeyState(VK_SHIFT) & 128;
	setKey(36, shift);
	if(shift) {
		remapping(remapShift, key, pressed);
	} else {
		int control = ::GetKeyState(VK_CONTROL) & 128;
		setKey(0, control);
		remapping(control ? remapCtrl : remapNone, key, pressed);
	}
}
