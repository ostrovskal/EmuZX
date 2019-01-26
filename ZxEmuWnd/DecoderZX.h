
#pragma once

#include "CpuZX.h"
#include "GpuZX.h"

extern ssh_b flags_cond[4];
extern ssh_b cnvPrefixSP[];
extern ssh_b cnvPrefixAF[];
extern GpuZX* gpu;

class DecoderZX {
	friend class CpuZX;
public:
	// �����������
	DecoderZX() { }
	
	// ����������
	virtual ~DecoderZX() { }

	// ����������� ��������� �������� � ������ ��������
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

	void noni() { _STATE &= ~ZX_INT; }

	inline void incrementR() {
		ssh_b r1 = _R & 127;
		_R = (_R & 128) | (++r1 & 127);
	}

protected:
	// ������ ��������
	inline ssh_b readOps() { 
		ssh_b v = read_mem8PC();
		typeOps = (v & 0b00000111);
		ops = (v & 0b00111000) >> 3;
		return ((v & 0b11000000) >> 6);
	}

	// ��������� �� ������� �����
	bool isFlag(int num) { return ((regsZX[RF] & flags_cond[(num & 7) >> 1]) ? 1 : 0) == (num & 1); }

	// �������� �������� �����
	ssh_b getFlag(ssh_b fl) { return ((regsZX[RF] & fl) == fl); }

	// ��������� ������ �� �����
	void update_flags(int msk, ssh_b val) { regsZX[RF] = (regsZX[RF] & ~msk) | val; }

	// ������ 16 ��� �� ������
	inline ssh_w read_mem16(ssh_w address) { return *(ssh_w*)(memZX + address); }

	// ������ 16 ��� �� ������
	inline ssh_w read_mem16PC() { ssh_w mem = *(ssh_w*)(memZX + _PC); _PC += 2; return mem; }

	// ������ 8 ��� �� ������
	inline ssh_b read_mem8(ssh_w address) { return memZX[address]; }

	// ������ 8 ��� �� ������
	inline ssh_b read_mem8PC() { return memZX[_PC++]; }

	// ����� � ������ 8 ������ ��������
	void write_mem8(ssh_b* address, ssh_b val, ssh_w ron) {
		if(address < &memZX[16384]) {
			write_rom(address); 
			return; 
		} 
		else if(address < &memZX[23296]) gpu->write(address, val);
		*address = val;
	}

	// ����� � ������ 8 ������ ��������
	inline void write_mem8(ssh_w address, ssh_b val) { write_mem8(&memZX[address], val, 6); }

	// ����� � ������ 16 ������ ��������
	void write_mem16(ssh_w address, ssh_w val) {
		ssh_b* mem = &memZX[address];
		if(address < 16384) { write_rom(&memZX[address]); return; }
		else if(address < 23296) {
			gpu->write(mem, (ssh_b)val);
			gpu->write(mem + 1, val >> 8);
		}
		*(ssh_w*)mem = val;
	}

	// ����������� 2 ������ ����� ����������� ���� � ��������� �� �������(� ������ SP)
	inline ssh_w* fromRP_SP(ssh_b rp) { return (ssh_w*)&regsZX[cnvPrefixSP[(prefix << 3) + (RC + (rp & 6))]]; }
	
	// ����������� 2 ������ ����� ����������� ���� � ��������� �� �������(� ������ AF)
	inline ssh_w* fromRP_AF(ssh_b rp) { return (ssh_w*)&regsZX[cnvPrefixAF[(prefix << 3) + (RC + (rp & 6))]]; }

	// ����������� 3 ������ ����� ��������� �������� � ��������� �� �������
	inline ssh_b* fromRON(ssh_b ron) {
		if(ron == 6) return (preg = &memZX[*fromRP_SP(4) + (char)(prefix ? read_mem8PC() : 0)]);
		return &regsZX[cnvPrefixAF[(prefix << 3) + (RC + ((ron & 7) ^ 1))]];
	}

	// ����������� 3 ������ ����� ��������� �������� � ��������� �� �������
	inline ssh_b* fromRON1(ssh_b ron) {
		if(ron == 6) return &memZX[*_HL];
		return &regsZX[(RC + ((ron & 7) ^ 1))];
	}

	// ����� ���������� (HL)
	ssh_b* preg;

	// ������� �������� DD/FD
	ssh_b prefix = 0;

	// ��� ��������
	ssh_b typeOps = 0;

	// ������� ��������
	ssh_b ops = 0;
private:
	// �������������� �������� � �������������
	void opsAccum(ssh_b d);

	// ������
	ssh_b rotate(ssh_b val, bool ed);

	// ����� ������������
	void execCALL(ssh_w address);

	// ����� �������� ���������
	void swapReg(ssh_w* r1, ssh_w* r2);

	// ���������� ��������
	void execOps(int prefix1, int prefix2);

	// ������ � ROM(���������)
	void write_rom(ssh_b* address) {}

	inline void flagsIR(ssh_b bt) { update_flags(FS | FZ | F5 | FH | F3 | FPV | FN, (bt & 128) | GET_FZ(bt) | (bt & 0b00101000) | (_IFF2 << 2)); }

	inline void nop() { }

	void writePort(ssh_w address, ssh_b val);

	ssh_b readPort(ssh_w address);
};

typedef void(DecoderZX::*funcDecoder)();


