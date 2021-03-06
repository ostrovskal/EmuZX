
#pragma once

#pragma comment (lib, "comctl32.lib")

// мои типы данных
typedef long long				ssh_l;
typedef unsigned long long		ssh_u;
typedef unsigned char			ssh_b;
typedef unsigned short			ssh_w;
typedef unsigned long			ssh_d;
typedef unsigned int			ssh_i;
typedef const char*				ssh_ccs;
typedef const wchar_t*			ssh_cws;
typedef char					ssh_cs;
typedef char*					ssh_pcs;
typedef wchar_t					ssh_ws;
typedef wchar_t*				ssh_pws;

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

#define RFMT_CODE		0	// последний код инструкции
#define RFMT_CODE_		2	// код инструкции в цепочке
#define RFMT_IADDR16	4	// 16 битный адрес в инструкции
#define RFMT_IADDR8		6	// 8 битный адрес в инструкции
#define RFMT_OFFS_P		8	// смещение +
#define RFMT_NUM16		10	// выравненное число
#define RFMT_CADDR		12	// вычисленный адрес
#define RFMT_PADDR		14	// содержимое адреса
#define RFMT_OPS8		16	// 8 битное значение операнда
#define RFMT_NUM		18	// число
#define RFMT_OFFS_M		20	// смещение -
#define RFMT_OPS16		22	// 16 битное значение операнда

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

void* ssh_memset(void* ptr, int set, ssh_u count);
void* ssh_memzero(void* ptr, ssh_u count);
void* ssh_memcpy(void* dst, const void* src, ssh_u count);
ssh_u ssh_pow2(ssh_u val, bool nearest);

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
	void*	asm_ssh_wton(const void* str, ssh_u radix, ssh_ws* end = nullptr);
	ssh_u	asm_ssh_length_last_number();
	ssh_cws asm_ssh_ntow(const void* ptr, ssh_u radix, ssh_ws** end = nullptr);
	ssh_ws* asm_ssh_parse_spec(void* arg, ssh_cws* str);
	ssh_l	asm_ssh_parse_xml(ssh_ws* src, ssh_w* vec);
	ssh_w	modifyTSTATE(int a, int r);
	void	makeTblParity();
	bool	unpackBlock(ssh_b* ptr, ssh_b* dst, ssh_b* dstE, int sz, bool packed, bool sign);
	bool	msgPump();
	bool	dlgSaveOrOpenFile(bool bOpen, ssh_cws title, ssh_cws filter, ssh_cws defExt, zxString& folder);
	ssh_b*	get_mem(ssh_w address);
	ssh_b*	packBlock(ssh_b* src, ssh_b* srcE, ssh_b* dst, bool sign, ssh_d& newSize);
//	ssh_cws fromNum(int num, ssh_cws fmt);
}

struct THREAD_STATE {
	HHOOK hHookOldCbtFilter;
	zxWnd* wndInit;
	MSG msg;
};

extern zxFrame*		theApp;
extern HINSTANCE	hInst;
extern ssh_b*		TR_DOS;

//extern ssh_ws*		tmpBuf;
//extern ssh_cws		radix[];
extern THREAD_STATE thState;
extern ssh_b		TMP_BUF[131072];
extern ssh_d		HEX;

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
