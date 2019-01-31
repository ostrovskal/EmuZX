
#pragma once

#include "CpuZX.h"

extern ssh_b flags_cond[4];
extern ssh_b cnvPrefixSP[];
extern ssh_b cnvPrefixAF[];

class DecoderZX {
	friend class CpuZX;
public:
	// Конструктор
	DecoderZX() { 
		debug = theApp.debug;
		gpu = theApp.gpu;
	}
	
	// Деструктор
	virtual ~DecoderZX() { }

	// обработчики категорий операций с учетом префикса
	void ops00_NO();
	void ops00_CB();
	void ops01_NO();
	void ops01_CB();
	void ops01_ED();
	void ops10_NO();
	void ops10_CB();
	void ops10_ED();
	void ops11_NO();
	void ops11_CB();
	void opsXX_ED();
	
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

	void noni() { modifyTSTATE(0, ZX_INT); }

	inline void incrementR() {
		ssh_b fs = (*_R) & 128;
		(*_R)++;
		*_R = ((*_R) & 127) | fs;
	}

	// выполнение операции
	void execOps(int prefix1, int prefix2);
protected:
	GpuZX*		gpu;
	zxDebugger* debug;
	// читаем операцию
	inline ssh_b readOps() { 
		ssh_b v = read_mem8PC();
		typeOps = (v & 0b00000111);
		ops = (v & 0b00111000) >> 3;
		return ((v & 0b11000000) >> 6);
	}

	// проверить на наличие флага
	bool isFlag(int num) { return ((regsZX[RF] & flags_cond[(num & 7) >> 1]) ? 1 : 0) == (num & 1); }

	// получить значение флага
	ssh_b getFlag(ssh_b fl) { return ((regsZX[RF] & fl) == fl); }

	// установка флагов по маске
	void update_flags(int msk, ssh_b val) { regsZX[RF] = (regsZX[RF] & ~msk) | val; }

	// читаем 16 бит из памяти
	inline ssh_w read_mem16(ssh_w address) { return *(ssh_w*)(memZX + address); }

	// читаем 16 бит из памяти
	inline ssh_w read_mem16PC() { ssh_w mem = *(ssh_w*)(memZX + _PC); _PC += 2; return mem; }

	// читаем 8 бит из памяти
	inline ssh_b read_mem8(ssh_w address) { return memZX[address]; }

	// читаем 8 бит из памяти
	inline ssh_b read_mem8PC() { return memZX[_PC++]; }

	// пишем в память 8 битное значение
	inline void write_mem8(ssh_w address, ssh_b val) { write_mem8(&memZX[address], val, 6); }

	// пишем в память 8 битное значение
	void write_mem8(ssh_b* address, ssh_b val, ssh_w ron);

	// пишем в память 16 битное значение
	void write_mem16(ssh_w address, ssh_w val);

	// преобразуем 2 битный номер регистровой пары в указатель на регистр(с учетом SP)
	inline ssh_w* fromRP_SP(ssh_b rp) { return (ssh_w*)&regsZX[cnvPrefixSP[(prefix << 3) + (RC + (rp & 6))]]; }
	
	// преобразуем 2 битный номер регистровой пары в указатель на регистр(с учетом AF)
	inline ssh_w* fromRP_AF(ssh_b rp) { return (ssh_w*)&regsZX[cnvPrefixAF[(prefix << 3) + (RC + (rp & 6))]]; }

	// преобразуем 3 битный номер половинки регистра в указатель на регистр
	inline ssh_b* fromRON(ssh_b ron) {
		if(ron == 6) return (preg = &memZX[*fromRP_SP(4) + (char)(prefix ? read_mem8PC() : 0)]);
		return &regsZX[cnvPrefixAF[(prefix << 3) + (RC + ((ron & 7) ^ 1))]];
	}

	// преобразуем 3 битный номер половинки регистра в указатель на регистр
	inline ssh_b* fromRON1(ssh_b ron) {
		if(ron == 6) return &memZX[*_HL];
		return &regsZX[(RC + ((ron & 7) ^ 1))];
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

	// сдвиги
	ssh_b rotate(ssh_b val, bool ed);

	// вызов подпрограммы
	void execCALL(ssh_w address);

	// запись в ROM(запрещено)
	void write_rom(ssh_b* address) {}

	inline void flagsIR(ssh_b bt) { update_flags(FS | FZ | F5 | FH | F3 | FPV | FN, (bt & 128) | GET_FZ(bt) | (bt & 0b00101000) | ((*_IFF2) << 2)); }

	inline void nop() { }

	void writePort(ssh_w address, ssh_b val);

	ssh_b readPort(ssh_w address);
};

typedef void(DecoderZX::*funcDecoder)();


