
#pragma once

extern ssh_b flags_cond[4];
extern ssh_d cnvPrefAF[];
extern ssh_d cnvPrefSP[];
extern ssh_d cnvPrefRON[];

enum CPU_REG {
	RC = 65536, RB, RE, RD, RL, RH, RF, RA,
	RC_, RB_, RE_, RD_, RL_, RH_, RF_, RA_,
	RXL, RXH, RYL, RYH,
	RSPL, RSPH, RPCL, RPCH,
	RI, RR, IFF1, IFF2, IM, TRAP,
	FE, LOCK_FD, KEMPSTON,
	RAM, ROM, VID, SCAN,
	PC_EXIT_CALL1, PC_EXIT_CALL2,
	MODEL, TSTATE1, TSTATE2,
	KEYS0, KEYS1, KEYS2, KEYS3, KEYS4, KEYS5, KEYS6, KEYS7
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
#define _FP(val)				calcFP(val)
#define _FN(val)				(val << 1)
#define _FC(val1, val2)			(val1 > val2)
#define _DA(c)					path[pos++] = c

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
extern ssh_w* _PC_EXIT_CALL;
extern ssh_w* _TSTATE;

extern ssh_b* _PORT_FE;
extern ssh_b* _LOCK_FD;
extern ssh_b* _MODEL;

extern ssh_b* _RAM;
extern ssh_b* _VID;
extern ssh_b* _ROM;
extern ssh_b* _SCAN;
extern ssh_b* _KEYS;
extern ssh_b* _KEMPSTON;
extern ssh_b memZX[50 + 65536];

class zxCPU {
	friend class zxBus;
public:
	// Конструктор
	zxCPU() { 
	}
	
	// Деструктор
	virtual ~zxCPU() {
	}

	// обработчики категорий операций с учетом префикса
	void opsNN00();
	void opsNN01();
	void opsNN10();
	void opsNN11();
	void opsCB00();
	void opsCB01();
	void opsCB10();
	void opsCB11();
	void opsED01();
	void opsED10();
	void opsEDXX();
	
	// opsN11_XXX
	void funcN11_000();
	void funcN11_001();
	void funcN11_011();
	void funcN11_101();

	// opsN00_XXX
	void ldRp();
	void incRp();
	void incSSS();
	void ldSSS();
	void funcN00_010();
	void funcN00_000();
	void funcN00_111();

	// opsED01_XXX
	void funcED01_000();
	void funcED01_111();

	void noni();

	inline void incrementR() {
		ssh_b fs = (*_R) & 128;
		(*_R)++;
		*_R = ((*_R) & 127) | fs;
	}

	// выполнение операции
	void execOps(int prefix1, int prefix2);
protected:
	// читаем операцию
	inline ssh_b readOps() { 
		ssh_b v = read_mem8PC();
		typeOps = (v & 0b00000111);
		ops = (v & 0b00111000) >> 3;
		return ((v & 0b11000000) >> 6);
	}

	// проверить на наличие флага
	bool isFlag(int num) { return ((memZX[RF] & flags_cond[(num & 7) >> 1]) ? 1 : 0) == (num & 1); }

	// получить значение флага
	ssh_b getFlag(ssh_b fl) { return ((memZX[RF] & fl) == fl); }

	// установка флагов по маске
	void update_flags(int msk, ssh_b val) { memZX[RF] = ((memZX[RF] & ~msk) | val); }

	// читаем 16 бит из памяти
	inline ssh_w read_mem16(ssh_w address) { return *(ssh_w*)(memZX + address); }

	// читаем 16 бит из памяти
	inline ssh_w read_mem16PC() { ssh_w mem = *(ssh_w*)(memZX + *_PC); (*_PC) += 2; return mem; }

	// читаем 8 бит из памяти
	inline ssh_b read_mem8(ssh_w address) { return memZX[address]; }

	// читаем 8 бит из памяти
	inline ssh_b read_mem8PC() { return memZX[(*_PC)++]; }

	// пишем в память 8 битное значение
	void write_mem8(ssh_b* address, ssh_b val);

	// пишем в память 16 битное значение
	void write_mem16(ssh_b* address, ssh_w val);

	// пишем в память 8 битное значение
	void write_mem8(ssh_w address, ssh_b val) { write_mem8(&memZX[address], val); }

	// пишем в память 8 битное значение
	void write_mem16(ssh_w address, ssh_w val) { write_mem16(&memZX[address], val); }

	// преобразуем 2 битный номер регистровой пары в указатель на регистр(с учетом SP)
	inline ssh_w* fromRP_SP(ssh_b rp) { return (ssh_w*)&memZX[cnvPrefSP[prefix + (rp & 6)]]; }
	
	// преобразуем 2 битный номер регистровой пары в указатель на регистр(с учетом AF)
	inline ssh_w* fromRP_AF(ssh_b rp) { return (ssh_w*)&memZX[cnvPrefAF[prefix + (rp & 6)]]; }

	// преобразуем 3 битный номер половинки регистра в указатель на регистр
	inline ssh_b* fromPrefRON(ssh_b ron) {
		if(ron == 6) return (preg = &memZX[(*fromRP_SP(4) + (char)(prefix ? read_mem8PC() : 0) & 0xffff)]);
		return &memZX[cnvPrefRON[prefix + ron]];
	}

	// преобразуем 3 битный номер половинки регистра в указатель на регистр
	inline ssh_b* fromRON(ssh_b ron) {
		if(ron == 6) return &memZX[*_HL];
		return &memZX[cnvPrefRON[ron]];
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
	void opsAccum(ssh_b d);

	// SBC/ADC
	void opsAccum2();

	// сдвиги
	ssh_b rotate(ssh_b val, ssh_b mask);

	// вызов подпрограммы
	void execCALL(ssh_w address);

	// запись в ROM(запрещено)
	void write_rom(ssh_b* address) {}

	inline void flagsIR(ssh_b bt) { update_flags(FS | FZ | FH | FPV | FN, _FS(bt) | _FZ(bt) | ((*_IFF2) << 2)); }

	inline void nop() { }
};

typedef void(zxCPU::*funcCPU)();


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
		if((o2 > 0 && (o1 > (mx - o2))) ||
			(o2 < 0 && (o1 < (mn - o2)))) return 4;
		return 0;
	}
	if((o2 > 0 && (o1 < (mn + o2))) ||
		(o2 < 0 && (o1 >(mx + o2)))) return 4;
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
	ssh_b o = op2 & 15;
	ssh_b ret = fn ? v - (o + fc) : v + (o + fc);
	return (ret > 15) << 4;
}

// обмен регистров
inline void swapReg(ssh_w* r1, ssh_w* r2) {
	ssh_w a = *r1;
	ssh_w b = *r2;
	*r2 = a;
	*r1 = b;
}
