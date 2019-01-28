#pragma once

#define DA_FHEX				0
#define DA_FDEC				1
#define DA_FCODE			2
#define DA_FADDRESS			4
#define DA_FADDR			8
#define DA_FPADDR			16

#define C_RB				0
#define C_RC				1
#define C_RD				2
#define C_RE				3
#define C_RH				4
#define C_RIXH				5
#define C_RIYH				6
#define C_RL				7
#define C_RIXL				8
#define C_RIYL				9

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
#define C_PRC				27
#define C_PNN				28
#define C_PN				29

#define C_NZ				30
#define C_Z					31
#define C_NC				32
#define C_C					33
#define C_PO				34
#define C_PE				35
#define C_P					36
#define C_M					37

#define C_NOP				38
#define C_EX_AF				39
#define C_DJNZ				40
#define C_JR				41

#define C_RLCA				42
#define C_RRCA				43
#define C_RLA				44
#define C_RRA				45
#define C_DAA				46
#define C_CPL				47
#define C_SCF				48
#define C_CCF				49

#define C_DI				50
#define C_EI				51

#define C_ADD				52
#define C_ADC				53
#define C_SUB				54
#define C_SBC				55
#define C_AND				56
#define C_XOR				57
#define C_OR				58
#define C_CP				59

#define C_ROT_RLC			60
#define C_ROT_RRC			61
#define C_ROT_RL			62
#define C_ROT_RR			63
#define C_ROT_SLA			64
#define C_ROT_SRA			65
#define C_ROT_SLI			66
#define C_ROT_SRL			67

#define C_BIT				68
#define C_RES				69
#define C_SET				70

#define C_INC				71
#define C_DEC				72

#define C_RRD				73
#define C_RLD				74

#define C_LDI				75
#define C_CPI				76
#define C_INI				77
#define C_OUTI				78

#define C_LDD				79
#define C_CPD				80
#define C_IND				81
#define C_OUTD				82

#define C_LDIR				83
#define C_CPIR				84
#define C_INIR				85
#define C_OUTIR				86

#define C_LDDR				87
#define C_CPDR				88
#define C_INDR				89
#define C_OUTDR				90

#define C_EXX				91
#define C_EX_DE				92
#define C_EX_SP				93
#define C_LD				94
#define C_JP				95
#define C_CALL				96
#define C_RET				97
#define C_RETF				98
#define C_RETI				99
#define C_RETN				100
#define C_RST				101
#define C_PUSH				102
#define C_POP				103
#define C_HALT				104
#define C_NEG				105
#define C_IN				106
#define C_OUT				107

#define C_IX_NONI			108
#define C_IY_NONI			109
#define C_ED_NONI			110
#define C_NULL				111

#define C_0					112
#define C_D					113
#define C_N					114
#define C_NN				115
#define C_COMMA				116
#define C_ADDR				117
#define C_PADDR				118

#define C_END				255

extern ssh_b prefAF[];
extern ssh_b prefSP[];
extern ssh_b prefRON[];
extern ssh_b offsRegs[];

class zxDisAsm {
public:
	zxDisAsm(): cmdCount(0), cmds(nullptr), adrs(nullptr) {}
	virtual ~zxDisAsm() { 
		delete cmds; cmds = nullptr; 
		delete adrs; adrs = nullptr;
	}
	
	// старт декодирования
	ssh_w decode(ssh_w pc, int count);

	// сохранение результата в файл
	bool save(ssh_cws path, ssh_b dec);

	// перемещение позиции вперед/назад
	ssh_w move(ssh_w pc, bool direct, int count);

	// формирование строки кода комманды
	StringZX makeCode(ssh_w address, int length, ssh_b dec);
	
	// формирование комманды
	StringZX makeCommand(int num, int flags);

	// вернуть адрес операнда комманды передачи управления(CALL, JP, JR)/загрузки или взятия адреса(LD A/RP, NN/LD A/RP,[NN]/ LD [NN], A/RP)
	int getCmdOperand(int num, bool pc);
	
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
	//ssh_w adr;
	int cmdCount;
	// читаем операцию
	inline ssh_b readOps() {
		ssh_b ops = memZX[_pc++];
		typeOps = (ops & 0b00000111);
		codeOps = (ops & 0b00111000) >> 3;
		groupOps = ((ops & 0b11000000) >> 6);
		return groupOps;
	}

	void execute(int prefix1, int prefix2);

	inline ssh_b fromRP_SP(ssh_b rp) { return prefSP[(prefix << 3) + (rp & 6)]; }

	inline ssh_b fromHL() { return C_PHL + prefix; }

	inline ssh_b fromRP_AF(ssh_b rp) { return prefAF[(prefix << 3) + (rp & 6)]; }

	inline ssh_b read8() { return memZX[_pc++]; }

	inline ssh_w read16() { auto v = *(ssh_w*)(memZX + _pc); _pc += 2; return v; }

	inline ssh_w addrReg(ssh_b reg) { return *(ssh_w*)(regsZX + offsRegs[reg - 10]); }

	inline void put16(ssh_w nn) { DA_PUT(nn & 255); DA_PUT(nn >> 8); }

	// B C D E H L (HL) A
	inline void fromRON1(ssh_b ron) { DA_PUT(prefRON[ron]); }
	
	// (HL/IX+d/IY+d)/B C D E H/IXH/IYH L/IXL/IYL
	void fromRON(ssh_b ron) {
		if(ron == 6) {
			auto r = fromHL();
			DA_PUT(r);
			if(prefix) { n = read8(); DA_PUT(C_D); DA_PUT(n); DA_PUT(C_ADDR); put16(addrReg(r) + (char)n); }
		} else DA_PUT(prefRON[(prefix << 3) + ron]);
	}

	ssh_b prefix;
	ssh_b typeOps;
	ssh_b codeOps;
	ssh_b groupOps;
	int pos;
	ssh_b path[16];
};

typedef void(zxDisAsm::*funcDA)();
