
#pragma once

extern ssh_b flags_cond[4];
extern ssh_b tbl_hcarry[16];
extern ssh_b tbl_overflow[16];
extern ssh_b tbl_parity[256];
extern ssh_b cnvPrefAF[];
extern ssh_b cnvPrefSP[];
extern ssh_b cnvPrefRON[];

enum CPU_REG {
	RC, RB, RE, RD, RL, RH, RF, RA,
	RC_, RB_, RE_, RD_, RL_, RH_, RF_, RA_,
	RXL, RXH, RYL, RYH,
	RSPL, RSPH, RPCL, RPCH,
	RI, RR, IFF1, IFF2, IM,
	PORT_FE, PORT_1F, PORT_FD, KEMPSTON,
	MOUSE0, MOUSE1, MOUSE2,
	TRDOS0, TRDOS1, TRDOS2, TRDOS3, TRDOS4, TRDOS5, TRDOS6, TRDOS7,
	FE16, FE8, AY_REG,
	AY0, AY1, AY2, AY3, AY4, AY5, AY6, AY7, AY8, AY9, AY10, AY11, AY12, AY13, AY14, AY15,
	TICK0, TICK1, TICK2, TICK3,
	RAM, ROM, VID,
	MODEL, TSTATE1, TSTATE2,
	KEYS0, KEYS1, KEYS2, KEYS3, KEYS4, KEYS5, KEYS6, KEYS7,
	COUNT_REGS
};

enum CPU_FLAGS {
	FC = 1, FN = 2, FPV = 4, FH = 16, FZ = 64, FS = 128
};

// префиксы
#define PREFIX_NO		0//0
#define PREFIX_CB		1//203
#define PREFIX_ED		2//237
#define PREFIX_IX		1//221
#define PREFIX_IY		2//253

#define _FS(val)				(val & 0x80)
#define _FZ(val)				((val == 0) << 6)
#define _FV1(op1, op2, fc, fn)	calcFV1(op1, op2, fc, fn)
#define _FV2(op1, op2, fc, fn)	calcFV2(op1, op2, fc, fn)
#define _FH(op1, op2, fc, fn)	calcFH(op1, op2, fc, fn)
#define _FP(val)				tbl_parity[val]
#define _FN(val)				(val << 1)
#define _FC(val1, val2)			(val1 > val2)
#define _DA(c)					path[pos++] = c

extern long szROM;
extern ssh_w RET_CALL;

extern ssh_b* _AY;
extern ssh_b* _I;
extern ssh_b* _R;
extern ssh_b* _IM;
extern ssh_b* _IFF1;
extern ssh_b* _IFF2;
extern ssh_b* _A;
extern ssh_b* _B;
extern ssh_w* _BC;
extern ssh_w* _DE;
extern ssh_w* _HL;
extern ssh_w* _AF;
extern ssh_w* _SP;
extern ssh_w* _PC;
extern ssh_w* _IX;
extern ssh_w* _IY;
extern ssh_w* _TSTATE;

extern ssh_d* _TICK;

extern ssh_b* _FE;
extern ssh_b* _7FFD;
extern ssh_b* _1FFD;
extern ssh_b* _MODEL;
extern ssh_b* _AY_REG;

extern ssh_b* _RAM;
extern ssh_b* _VID;
extern ssh_b* _ROM;
extern ssh_b* _KEYS;
extern ssh_b* _KEMPSTON;
extern ssh_b  cpuZX[COUNT_REGS];
extern ssh_b* pageROM;
extern ssh_b* pageRAM;
extern ssh_b* pageVRAM;
extern ssh_b* pageATTRIB;
extern ssh_b* PAGE_RAM[64];
extern ssh_b* PAGE_ROM[8];

extern "C" {
	ssh_b* get_mem(ssh_w address);
};

class zxCPU {
	friend class zxBus;
public:
	// обработчики категорий операций с учетом префикса
	int opsNN00();
	int opsNN01();
	int opsNN10();
	int opsNN11();
	int opsCB00();
	int opsCB01();
	int opsCB10();
	int opsCB11();
	int opsED01();
	int opsED10();
	int opsEDXX();
	
	// opsN11_XXX
	int funcN11_000();
	int funcN11_001();
	int funcN11_011();
	int funcN11_101();

	// opsN00_XXX
	int ldRp();
	int incRp();
	int incSSS();
	int ldSSS();
	int funcN00_010();
	int funcN00_000();
	int funcN00_111();

	// opsED01_XXX
	int funcED01_000();
	int funcED01_111();

	int noni();

	inline void incrementR() {
		ssh_b fs = (*_R) & 128;
		(*_R)++;
		*_R = ((*_R) & 127) | fs;
	}

	// выполнение операции
	int execOps(int prefix1, int prefix2);
protected:
	// читаем операцию
	inline ssh_b readOps() { 
		ssh_b v = read_mem8PC();
		typeOps = (v & 0b00000111);
		ops = (v & 0b00111000) >> 3;
		return ((v & 0b11000000) >> 6);
	}

	// проверить на наличие флага
	bool isFlag(int num) { return ((cpuZX[RF] & flags_cond[(num & 7) >> 1]) ? 1 : 0) == (num & 1); }

	// получить значение флага
	ssh_b getFlag(ssh_b fl) { return ((cpuZX[RF] & fl) == fl); }

	// установка флагов по маске
	void update_flags(int msk, ssh_b val) { cpuZX[RF] = ((cpuZX[RF] & ~msk) | val); }
	
	// читаем 16 бит из памяти
	inline ssh_w read_mem16PC() { return (read_mem8PC() + (read_mem8PC() << 8)); }

	// читаем 8 бит из памяти
	inline ssh_b read_mem8PC() { return *get_mem((*_PC)++); }

	// пишем в память 8 битное значение
	void write_mem8(ssh_w address, ssh_b val);

	// пишем в память 16 битное значение
	void write_mem16(ssh_w address, ssh_w val);

	// преобразуем 2 битный номер регистровой пары в указатель на регистр(с учетом SP)
	inline ssh_w* fromRP_SP(ssh_b rp) { return (ssh_w*)&cpuZX[cnvPrefSP[prefix + (rp & 6)]]; }
	
	// преобразуем 2 битный номер регистровой пары в указатель на регистр(с учетом AF)
	inline ssh_w* fromRP_AF(ssh_b rp) { return (ssh_w*)&cpuZX[cnvPrefAF[prefix + (rp & 6)]]; }

	// преобразуем 3 битный номер половинки регистра в указатель на регистр
	inline ssh_b* fromPrefRON(ssh_b ron) {
		if (ron == 6) {
			ssh_w address = *fromRP_SP(4) + (char)(prefix ? read_mem8PC() : 0);
			return (preg = get_mem(address));
		}
		return &cpuZX[cnvPrefRON[prefix + ron]];
	}

	// преобразуем 3 битный номер половинки регистра в указатель на регистр
	inline ssh_b* fromRON(ssh_b ron) {
		if (ron == 6) return get_mem(*_HL);
		return &cpuZX[cnvPrefRON[ron]];
	}

	// адрес последнего (HL)
	ssh_b* preg;

	// префикс комманды DD/FD
	ssh_b prefix = 0;

	// тип операции
	ssh_b typeOps = 0;

	// текущая операция
	ssh_b ops = 0;
private:
	// арифметические операции с аккумулятором
	int opsAccum(ssh_b d);

	// SBC/ADC
	int opsAccum2();

	// сдвиги
	ssh_b rotate(ssh_b val, ssh_b mask);

	// вызов подпрограммы
	int execCALL(ssh_w address);

	inline void flagsIR(ssh_b bt) { update_flags(FS | FZ | FH | FPV | FN, _FS(bt) | _FZ(bt) | ((*_IFF2) << 2)); }

	inline int nop() { return 4; }
};

typedef int (zxCPU::*funcCPU)();
