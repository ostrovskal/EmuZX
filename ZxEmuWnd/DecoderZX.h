
#pragma once

#include "CpuZX.h"

extern byte flags_cond[4];
extern byte cnvPrefixSP[];
extern byte cnvPrefixAF[];

class GpuZX;
class DecoderZX {
	friend class CpuZX;
public:
	// Конструктор
	DecoderZX() { }
	
	// Деструктор
	virtual ~DecoderZX() { }

	// обработчики категорий операций с учетом префикса
	void ops00_NO();
	void ops00_CB();
	void opsXX_ED();
	void ops01_NO();
	void ops01_CB();
	void ops01_ED();
	void ops10_NO();
	void ops10_CB();
	void ops10_ED();
	void ops11_NO();
	void ops11_CB();
	
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

	GpuZX* gpu;
protected:
	// читаем операцию
	inline byte readOps() { 
		byte v = read_mem8PC();
		typeOps = (v & 0b00000111);
		ops = (v & 0b00111000) >> 3;
		return ((v & 0b11000000) >> 6);
	}

	// проверить на наличие флага
	bool isFlag(int num) { return ((CpuZX::cpu[RF] & flags_cond[(num & 7) >> 1]) ? 1 : 0) == (num & 1); }

	// получить значение флага
	byte getFlag(byte fl) { return ((CpuZX::cpu[RF] & fl) ? 1 : 0); }

	// установка флагов по маске
	void update_flags(int msk, byte val) { CpuZX::cpu[RF] = (CpuZX::cpu[RF] & ~msk) | val; }

	// читаем 16 бит из памяти
	inline word read_mem16(word address) { return *(word*)(CpuZX::memory + address); }

	// читаем 16 бит из памяти
	inline word read_mem16PC() { word mem = *(word*)(CpuZX::memory + CpuZX::RPC); CpuZX::RPC += 2; return mem; }

	// читаем 8 бит из памяти
	inline byte read_mem8(word address) { return CpuZX::memory[address]; }

	// читаем 8 бит из памяти
	inline byte read_mem8PC() { return CpuZX::memory[CpuZX::RPC++]; }

	// пишем в память 8 битное значение
	void write_mem8(byte* address, byte val, word ron) {
		if(address < &CpuZX::memory[16384]) {
			write_rom(address); 
			return; 
		} 
		else if(address < &CpuZX::memory[23296]) gpu->write(address, val);
		*address = val;
	}

	// пишем в память 8 битное значение
	inline void write_mem8(word address, byte val) { write_mem8(&CpuZX::memory[address], val, 6); }

	// пишем в память 16 битное значение
	void write_mem16(word address, word val) {
		byte* mem = &CpuZX::memory[address];
		if(address < 16384) { write_rom(&CpuZX::memory[address]); return; }
		else if(address < 23296) {
			gpu->write(mem, (byte)val);
			gpu->write(mem + 1, val >> 8);
		}
		*(word*)mem = val;
	}

	// преобразуем 2 битный номер регистровой пары в указатель на регистр(с учетом SP)
	inline word* fromRP_SP(int rp) { return (word*)&CpuZX::cpu[cnvPrefixSP[(prefix << 3) + (RC + (rp & 6))]]; }
	
	// преобразуем 2 битный номер регистровой пары в указатель на регистр(с учетом AF)
	inline word* fromRP_AF(int rp) { return (word*)&CpuZX::cpu[cnvPrefixAF[(prefix << 3) + (RC + (rp & 6))]]; }

	// преобразуем 3 битный номер половинки регистра в указатель на регистр
	inline byte* fromRON(int ron) {
		if(ron == 6) return (preg = &CpuZX::memory[*fromRP_SP(4) + (char)(prefix ? read_mem8PC() : 0)]);
		return &CpuZX::cpu[cnvPrefixAF[(prefix << 3) + (RC + ((ron & 7) ^ 1))]];
	}

	// преобразуем 3 битный номер половинки регистра в указатель на регистр
	inline byte* fromRON1(int ron) {
		if(ron == 6) return &CpuZX::memory[*CpuZX::HL];
		return &CpuZX::cpu[(RC + ((ron & 7) ^ 1))];
	}

	// адрес последнего (HL)
	byte* preg;

	// префикс комманды DD/FD
	byte prefix = 0;

	// тип операции
	byte typeOps = 0;

	// текущая операция
	byte ops = 0;
private:
	// арифметические операции с аккумулятором
	void opsAccum(byte d);

	// сдвиги
	byte rotate(byte val, int msk);

	// вызов подпрограммы
	void execCALL(word address);

	// обмен значений регистров
	void swapReg(word* r1, word* r2);

	// выполнение операции
	void execOps(int prefix1, int prefix2);

	// запись в ROM(запрещено)
	void write_rom(byte* address) {}

	inline void flagsIR(byte bt) { update_flags(FS | FZ | F5 | FH | F3 | FPV | FN, (bt & 128) | GET_FZ(bt) | (bt & 0b00101000) | (CpuZX::IFF2 << 2)); }

	inline void noni() { CpuZX::STATE &= ~CpuZX::INT; }

	inline void nop() { }
};

typedef void(DecoderZX::*funcOps)();


