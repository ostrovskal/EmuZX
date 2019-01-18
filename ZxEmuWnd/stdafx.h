// stdafx.h: включаемый файл для стандартных системных включаемых файлов
// или включаемых файлов для конкретного проекта, которые часто используются, но
// не часто изменяются
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Исключите редко используемые компоненты из заголовков Windows
// Файлы заголовков Windows:
#include <windows.h>

// Файлы заголовков C RunTime
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <fcntl.h>
#include <io.h>
#include <timeapi.h>

typedef unsigned int dword;
typedef unsigned short word;
typedef unsigned char byte;

enum CPU_REG {
	RC, // 0
	RB, // 1
	RE, // 2
	RD, // 3
	RL, // 4
	RH, // 5
	RA, // 6
	RF, // 7 (HL)
	RIXL, // 8
	RIXH, // 9
	RIYL, // 10
	RIYH, // 11
	RSPL, // 12
	RSPH, // 13
	RBC,// 14
	RDE,// 15
	RHL,// 16
	RAF,// 17
	RIX,// 18
	RIY,// 19
	RSP// 20
};

enum CPU_FLAGS {
	FC = 1,
	FN = 2,// SUB,DEC,CMP
	FPV =4,//P=1->четность бит,V=1->переполнение
	F3 = 8,// не используется
	FH = 16,// перенос между 3 и 4 битами
	F5 = 32,// не используется
	FZ = 64,
	FS = 128
};

// префиксы
#define PREFIX_NO		0//0
#define PREFIX_CB		1//203
#define PREFIX_ED		2//237
#define PREFIX_IX		1//221
#define PREFIX_IY		2//253

#define GET_FZ(val)				(val == 0) << 6
#define GET_FV(val1, val2)		calcFV(val1, val2)
#define GET_FP(val)				calcFP(val) << 2
#define GET_FN(val)				val << 1
#define GET_FC(val1, val2)		(val1 > val2)

byte calcFP(byte val);
byte calcFV(byte val1, byte val2);
byte calcFH(byte val, byte offs, byte fn);