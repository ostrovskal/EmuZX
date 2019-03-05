
#pragma once

#pragma comment (lib, "comctl32.lib")

// мои типы данных
typedef long long						ssh_l;
typedef unsigned long long				ssh_u;
typedef unsigned char					ssh_b;
typedef unsigned short					ssh_w;
typedef unsigned long					ssh_d;
typedef unsigned int					ssh_i;
typedef const char*						ssh_ccs;
typedef const wchar_t*					ssh_cws;
typedef char							ssh_cs;
typedef char*							ssh_pcs;
typedef wchar_t							ssh_ws;
typedef wchar_t*						ssh_pws;

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN
#define _CRT_NON_CONFORMING_SWPRINTFS

#include <windows.h>
#include <WinUser.h>

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <fcntl.h>
#include <io.h>
#include <timeapi.h>
#include <shlobj.h>
#include <sys/stat.h>
#include <mmsystem.h>
#include <commdlg.h>
#include "resource.h"
#include <math.h>

using namespace std;

#include "zxList.h"
#include "zxFile.h"
#include "zxString.h"

#include "zxWnd.h"
#include "zxMsgMap.h"
#include "zxFrame.h"

enum ZX_TSTATES {
	ZX_INT			= 1,
	ZX_NMI			= 2,
	ZX_DEBUG		= 4,
	ZX_EXEC			= 8,
	ZX_HALT			= 16,
	ZX_RESET		= 32,
	ZX_TR_DOS		= 64
};

struct ZX_BREAK_POINT {
	ZX_BREAK_POINT() : address1(0), address2(0), flags(0), value(0), mask(0) {}
	int address1;
	int address2;
	int value;
	int flags;
	ssh_b mask;
};

struct ZX_KEY {
	int butID;
	ssh_b port;
	ssh_b bit;
	ssh_b portEx;
	ssh_b bitEx;
	ssh_b vk_code;
	ssh_cws name_k;
	ssh_cws name_l;
	ssh_cws name_c;
	ssh_cws name_e;
	ssh_cws name_sh_e;
	ssh_cws name_sh_k;
	ssh_cws name_g;
	HWND hWndKey;
};

struct ZX_KEY_EX {
	ssh_b vk_code;
	ssh_b vk_codeKey;
	ssh_b vk_codeKeyEx;
};

#define ZX_KEY_1		1
#define ZX_KEY_2		2
#define ZX_KEY_3		3
#define ZX_KEY_4		4
#define ZX_KEY_5		5
#define ZX_KEY_6		6
#define ZX_KEY_7		7
#define ZX_KEY_8		8
#define ZX_KEY_9		9
#define ZX_KEY_0		10
#define ZX_KEY_Q		11
#define ZX_KEY_W		12
#define ZX_KEY_E		13
#define ZX_KEY_R		14
#define ZX_KEY_T		15
#define ZX_KEY_Y		16
#define ZX_KEY_U		17
#define ZX_KEY_I		18
#define ZX_KEY_O		19
#define ZX_KEY_P		20
#define ZX_KEY_A		21
#define ZX_KEY_S		22
#define ZX_KEY_D		23
#define ZX_KEY_F		24
#define ZX_KEY_G		25
#define ZX_KEY_H		26
#define ZX_KEY_J		27
#define ZX_KEY_K		28
#define ZX_KEY_L		29
#define ZX_KEY_ENTER	30
#define ZX_KEY_CAPS		31
#define ZX_KEY_Z		32
#define ZX_KEY_X		33
#define ZX_KEY_C		34
#define ZX_KEY_V		35
#define ZX_KEY_B		36
#define ZX_KEY_N		37
#define ZX_KEY_M		38
#define ZX_KEY_SYMBOL	39
#define ZX_KEY_SPACE	40
#define ZX_KEY_UP		41
#define ZX_KEY_DOWN		42
#define ZX_KEY_LEFT		43
#define ZX_KEY_RIGHT	44
#define ZX_KEY_KEM_UP	45
#define ZX_KEY_KEM_DOWN	46
#define ZX_KEY_KEM_LEFT	47
#define ZX_KEY_KEM_RIGHT 48
#define ZX_KEY_KEM_FIRE 49

void* ssh_memset(void* ptr, int set, ssh_u count);
void* ssh_memzero(void* ptr, ssh_u count);
void* ssh_memcpy(void* dst, const void* src, ssh_u count);
ssh_u ssh_pow2(ssh_u val, bool nearest);

#define SAFE_RELEASE(ptr)		if(ptr) { (ptr)->Release(); (ptr) = nullptr; }
#define SAFE_DELETE(ptr)		if(ptr) { delete[] ptr; (ptr) = nullptr; }
#define SAFE_A_DELETE(ptr)		if(ptr) { delete[] ptr; (ptr) = nullptr; }

#define FBP_PC			1
#define FBP_MEM			2

#define FBP_EQ			2	// 0 ==
#define FBP_NEQ			4	// 1 !=
#define FBP_LS			8	// 2 <
#define FBP_GT			12	// 3 >
#define FBP_QLS			16	// 4 <=
#define FBP_QGT			20	// 5 >=

#define FBP_ADDR		32
#define FBP_VAL			64
#define FBP_MASK		128

#define FBP_ACCESS		3
#define FBP_COND		30

#define COUNT_BP		10
#define COUNT_STORY_PC	64

#define ST_EXECUTE		0x01
#define ST_TURBO		0x02
#define ST_JOYSTICK		0x04
#define ST_SOUND		0x08
#define ST_DEBUGGER		0x10
#define ST_KEYBOARD		0x20
#define ST_MODEL		0x40
#define ST_ASPECT		0x80
#define ST_FILTER		0x100
#define STS_EXECUTE		0x200
#define ST_EXECUTE_GO	0x400

#define SND_BEEP		0
#define SND_AY			1
#define SND_COVOX		2
#define SND_SPECDRUM	3

typedef void (zxBus::* funcBus)(int, int);

extern "C" {
	void	asm_ssh_zx_init(void* regs, void* mem, void* port, void* checkBPS);
	void	asm_ssh_zx_step();
	void	asm_ssh_post_process(ssh_d filter, void* rect, ssh_d* buffer, ssh_d* screen);
	void	asm_ssh_update_frame(int sizeBorder, int blink, ssh_d* dest, ssh_d* colors, void* bus, funcBus func, void* vram);
	ssh_b	asm_ssh_rotate(ssh_b v, ssh_u ops, ssh_b* fl);
	ssh_b	asm_ssh_accum(ssh_u op1, ssh_u op2, ssh_u ops, ssh_b* fl);
	ssh_w	asm_ssh_accum2(ssh_u op1, ssh_u op2, ssh_u ops, ssh_b* fl);
	ssh_u	asm_ssh_capability();
	ssh_cws asm_ssh_to_base64(ssh_b* ptr, ssh_u count);
	ssh_b*	asm_ssh_from_base64(ssh_ws* str, ssh_u count, ssh_u* len_buf);
	void*	asm_ssh_wton(ssh_cws str, ssh_u radix, ssh_ws* end = nullptr);
	ssh_u	asm_ssh_length_last_number();
	ssh_cws asm_ssh_ntow(const void* ptr, ssh_u radix, ssh_ws* end = nullptr);
	ssh_ws* asm_ssh_parse_spec(void* arg, ssh_cws* str);
	ssh_l	asm_ssh_parse_xml(ssh_ws* src, ssh_w* vec);
	ssh_w	modifyTSTATE(int a, int r);
	void	writePort(ssh_b A0A7, ssh_b A8A15, ssh_b val);
	ssh_b	readPort(ssh_b A0A7, ssh_b A8A15);
}

struct THREAD_STATE {
	HHOOK hHookOldCbtFilter;
	zxWnd* wndInit;
	MSG msg;
};

extern ZX_KEY keys[];
extern zxFrame* theApp;

extern HINSTANCE hInst;

extern ssh_b* TR_DOS;

extern ssh_ws* tmpBuf;
extern ssh_cws radix[];
extern ssh_cws cond_bp[];
extern ssh_cws nameROMs[];
extern THREAD_STATE thState;
extern ssh_b TMP_BUF[131072];

void makeTblParity();
ssh_b* get_mem(ssh_w address);
ssh_b* packBlock(ssh_b* src, ssh_b* srcE, ssh_b* dst, bool sign, ssh_d& newSize);
bool unpackBlock(ssh_b* ptr, ssh_b* dst, ssh_b* dstE, int sz, bool packed, bool sign);
bool msgPump();
ssh_cws fromNum(int num, ssh_cws fmt);

// вычисление четности
inline ssh_b calcFP(ssh_b val) {
	val = ((val >> 1) & 0x55) + (val & 0x55);
	val = ((val >> 2) & 0x33) + (val & 0x33);
	val = ((val >> 4) & 0x0F) + (val & 0x0F);
	return (!(val & 1) << 2);
}

// вычисление переполнения
inline ssh_b calcFV(int o1, int o2, int mx, int mn, ssh_w fn) {
	if(!fn) {
		if((o2 > 0 && (o1 > (mx - o2))) || (o2 < 0 && (o1 < (mn - o2)))) return 4;
	} else if((o2 > 0 && (o1 < (mn + o2))) || (o2 < 0 && (o1 > (mx + o2)))) return 4;
	return 0;
}

// вычисление переполнения
inline ssh_b calcFV1(char op1, char op2, ssh_b fc, ssh_w fn) {
	return calcFV(op1, op2 + fc, CHAR_MAX, CHAR_MIN, fn);
}

// вычисление переполнения
inline ssh_b calcFV2(short op1, short op2, ssh_b fc, ssh_w fn) {
	return calcFV(op1, op2 + fc, SHRT_MAX, SHRT_MIN, fn);
}

// вычисление полупереноса
inline ssh_b calcFH(ssh_b op1, ssh_b op2, ssh_b fc, ssh_b fn) {
	ssh_b v = op1 & 15;
	ssh_w o = (op2 & 15) + fc;
	ssh_b ret = fn ? v - o : v + o;
	return (ret > 15) << 4;
	/*
	ssh_b v = op1 & 15;
	ssh_b o = op2 & 15;
	ssh_b ret = fn ? v - (o + fc) : v + (o + fc);
	return (ret > 15) << 4;
	*/
}

// обмен регистров
inline void swapReg(ssh_w * r1, ssh_w * r2) {
	ssh_w a = *r1;
	ssh_w b = *r2;
	*r2 = a;
	*r1 = b;
}

// читаем 8 бит из памяти
inline ssh_b read_mem8(ssh_w address) { return *get_mem(address); }

// читаем 16 бит из памяти
inline ssh_w read_mem16(ssh_w address) { return (read_mem8(address) + (read_mem8(address + 1) << 8)); }

// пишем в память 8 битное значение
inline void write_mem8(ssh_b * address, ssh_b val) {
	if(address >= PAGE_ROM[0] && address < (PAGE_ROM[0] + szROM)) {
		// запись в ПЗУ
		if(pageROM != PAGE_RAM[0]) {
			// ошибка
			return;
		}
	}
	*address = val;
}
