#pragma once

#define DA_FCODE			1
#define DA_FADDRESS			2
#define DA_FADDR			4
#define DA_FPADDR			8

#define SIZE_CMD			16L

#define C_RB				0
#define C_RC				1
#define C_RD				2
#define C_RE				3
#define C_RH				4
#define C_RXH				5
#define C_RYH				6
#define C_RL				7
#define C_RXL				8
#define C_RYL				9

#define C_PHL				10
#define C_PIX				11
#define C_PIY				12

#define C_RA				13
#define C_RF				14

#define C_RBC				15
#define C_RDE				16
#define C_RHL				17
#define C_RAF				18
#define C_RSP				19
#define C_RIX				20
#define C_RIY				21
#define C_RR				22
#define C_RI				23
#define C_IM				24

#define C_PBC				25
#define C_PDE				26

#define C_NZ				27
#define C_Z					28
#define C_NC				29
#define C_C					30
#define C_PO				31
#define C_PE				32
#define C_P					33
#define C_M					34

#define C_NOP				35
#define C_EX_AF				36
#define C_DJNZ				37
#define C_JR				38

#define C_RLCA				39
#define C_RRCA				40
#define C_RLA				41
#define C_RRA				42
#define C_DAA				43
#define C_CPL				44
#define C_SCF				45
#define C_CCF				46

#define C_DI				47
#define C_EI				48

#define C_ADD				49
#define C_ADC				50
#define C_SUB				51
#define C_SBC				52
#define C_AND				53
#define C_XOR				54
#define C_OR				55
#define C_CP				56

#define C_ROT_RLC			57
#define C_ROT_RRC			58
#define C_ROT_RL			59
#define C_ROT_RR			60
#define C_ROT_SLA			61
#define C_ROT_SRA			62
#define C_ROT_SLI			63
#define C_ROT_SRL			64

#define C_BIT				65
#define C_RES				66
#define C_SET				67

#define C_INC				68
#define C_DEC				69

#define C_RRD				70
#define C_RLD				71

#define C_LDI				72
#define C_CPI				73
#define C_INI				74
#define C_OUTI				75

#define C_LDD				76
#define C_CPD				77
#define C_IND				78
#define C_OUTD				79

#define C_LDIR				80
#define C_CPIR				81
#define C_INIR				82
#define C_OUTIR				83

#define C_LDDR				84
#define C_CPDR				85
#define C_INDR				86
#define C_OUTDR				87

#define C_EXX				88
#define C_EX_DE				89
#define C_EX_SP				90
#define C_LD				91
#define C_JP				92
#define C_CALL				93
#define C_RET				94
#define C_RETF				95
#define C_RETI				96
#define C_RETN				97
#define C_RST				98
#define C_PUSH				99
#define C_POP				100
#define C_HALT				101
#define C_NEG				102
#define C_IN				103
#define C_OUT				104

#define C_IX_NONI			105
#define C_IY_NONI			106
#define C_ED_NONI			107

#define C_COMMA				108
#define C_NULL				109
#define C_0					110
#define C_PRC				111

#define C_PNN				112
#define C_PN				113
#define C_D					114
#define C_N					115
#define C_NN				116
#define C_ADDR				117
#define C_PADDR				118
#define C_BT				119

#define C_END				255

extern ssh_b prefAF[];
extern ssh_b prefSP[];
extern ssh_b prefRON[];
extern ssh_d offsRegs[];

class zxDisAsm {
public:
	zxDisAsm(): cmdCount(0), cmds(nullptr), adrs(nullptr) {}
	virtual ~zxDisAsm() { 
		delete[] cmds; cmds = nullptr; 
		delete[] adrs; adrs = nullptr;
	}
	
	// ����� �������������
	ssh_w decode(ssh_w pc, ssh_d count, ssh_d* realCount = nullptr);

	// ���������� ���������� � ����
	bool save(ssh_cws path);

	// ����������� ������� ������/�����
	ssh_w move(ssh_w pc, int count);

	// ������������ ������ ���� ��������
	zxString makeCode(ssh_w address, int length) const;
	
	// ������������ ��������
	zxString makeCommand(ssh_d num, int flags);

	// ������� ����� �������� �������� �������� ����������(CALL, JP, JR)/�������� ��� ������ ������(LD A/RP, NN/LD A/RP,[NN]/ LD [NN], A/RP)
	void getCmdOperand(ssh_d num, bool pc, bool call, bool ret, int* addr1, int* addr2);

	// ������� ����� ��������
	int getCmdAddress(ssh_d num) const;

	// ������� ������ �������� �� �� ������
	int indexFromAddress(ssh_d pc) const;
		
	ssh_b* cmds;
	ssh_w* adrs;

	void opsCB();
	
	void opsED01();
	void opsED10();
	void opsEDXX();
	
	void ops00();
	void ops01();
	void ops10();
	void ops11();
protected:
	ssh_w _pc;
	ssh_w nn;
	ssh_b n;
	ssh_d cmdCount;
	// ������ ��������
	inline ssh_b readOps() {
		ssh_b ops = read8();
		typeOps = (ops & 0b00000111);
		codeOps = (ops & 0b00111000) >> 3;
		groupOps = ((ops & 0b11000000) >> 6);
		return groupOps;
	}

	ssh_w cmdJump(ssh_w pc);

	void execute(int prefix1, int prefix2);

	inline ssh_b fromRP_SP(ssh_b rp) { return prefSP[prefix + (rp & 6)]; }

	inline ssh_b fromHL() { return C_PHL + (prefix >> 3); }

	inline ssh_b fromRP_AF(ssh_b rp) { return prefAF[prefix + (rp & 6)]; }

	inline ssh_b read8() { return *get_mem(_pc++); }

	inline ssh_w read16() { return (read8() + (read8() << 8)); }

	inline ssh_w addrReg(ssh_b reg) { return *(ssh_w*)(cpuZX + offsRegs[reg - 10]); }

	inline void put16(ssh_w nn) { _DA(nn & 255); _DA(nn >> 8); }

	// B C D E H L (HL) A
	inline void fromRON(ssh_b ron) { _DA(prefRON[ron]); }
	
	// (HL/IX+d/IY+d)/B C D E H/XH/YH L/XL/YL
	void fromPrefRON(ssh_b ron) {
		if(ron == 6) {
			auto r = fromHL();
			_DA(r);
			if(prefix) { n = read8(); _DA(C_D); _DA(n); _DA(C_ADDR); auto addr = addrReg(r); put16((addr + (char)n) & 0xffff); }
		} else _DA(prefRON[prefix + ron]);
	}

	ssh_b prefix;
	ssh_b typeOps;
	ssh_b codeOps;
	ssh_b groupOps;
	int pos;
	ssh_b path[16];
};

typedef void(zxDisAsm::*funcDA)();
