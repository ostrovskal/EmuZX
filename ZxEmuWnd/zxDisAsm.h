#pragma once

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
#define C_D					27
#define C_N					28
#define C_NN				29
#define C_COMMA				30

#define C_NZ				31
#define C_Z					32
#define C_NC				33
#define C_C					34
#define C_PO				35
#define C_PE				36
#define C_P					37
#define C_M					38

#define C_NOP				39
#define C_EX_AF				40
#define C_DJNZ				41
#define C_JR				42

#define C_RLCA				43
#define C_RRCA				44
#define C_RLA				45
#define C_RRA				46
#define C_DAA				47
#define C_CPL				48
#define C_SCF				49
#define C_CCF				50

#define C_DI				51
#define C_EI				52

#define C_ADD				53
#define C_ADC				54
#define C_SUB				55
#define C_SBC				56
#define C_AND				57
#define C_XOR				58
#define C_OR				59
#define C_CP				60

#define C_ROT_RLC			61
#define C_ROT_RRC			62
#define C_ROT_RL			63
#define C_ROT_RR			64
#define C_ROT_SLA			65
#define C_ROT_SRA			66
#define C_ROT_SLI			67
#define C_ROT_SRL			68

#define C_BIT				69
#define C_RES				70
#define C_SET				71

#define C_INC				72
#define C_DEC				73

#define C_RRD				74
#define C_RLD				75

#define C_LDI				76
#define C_CPI				77
#define C_INI				78
#define C_OUTI				79

#define C_LDD				80
#define C_CPD				81
#define C_IND				82
#define C_OUTD				83

#define C_LDIR				84
#define C_CPIR				85
#define C_INIR				86
#define C_OUTIR				87

#define C_LDDR				88
#define C_CPDR				89
#define C_INDR				90
#define C_OUTDR				91

#define C_EXX				92
#define C_EX_DE				93
#define C_EX_SP				94
#define C_LD				95
#define C_JP				96
#define C_CALL				97
#define C_RET				98
#define C_RETF				99
#define C_RETI				100
#define C_RETN				101
#define C_RST				102
#define C_PUSH				103
#define C_POP				104
#define C_HALT				105
#define C_NEG				106
#define C_IN				107
#define C_OUT				108

#define C_IX_NONI			109
#define C_IY_NONI			110
#define C_ED_NONI			111
#define C_NULL				112

#define C_0					113
#define C_PRC				114
#define C_PNN				115
#define C_PN				116
#define C_ADDR				117
#define C_PADDR				118

extern ssh_b prefAF[];
extern ssh_b prefSP[];
extern ssh_b prefRON[];

class zxDisAsm {
public:
	zxDisAsm(): cmdCount(0), cmds(nullptr), codes(nullptr), adrs(nullptr) {}
	virtual ~zxDisAsm() { 
		delete cmds; cmds = nullptr; 
		delete codes; codes = nullptr;
		delete adrs; adrs = nullptr;
	}
	
	ssh_w decode(ssh_w pc, int count, bool isDecimal);
	bool save(ssh_cws path);

	StringZX* cmds;
	StringZX* adrs;
	StringZX* codes;

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
	ssh_b d, n;
	ssh_w adr;
	int cmdCount;
	// читаем операцию
	inline ssh_b readOps() {
		ssh_b ops = memZX[_pc++];
		path_ops[posOps++] = ops;
		typeOps = (ops & 0b00000111);
		codeOps = (ops & 0b00111000) >> 3;
		groupOps = ((ops & 0b11000000) >> 6);
		return groupOps;
	}

	void execute(int prefix1, int prefix2);

	inline ssh_b fromRP_SP(ssh_b rp) { return prefSP[(prefix << 3) + (rp & 6)]; }

	inline ssh_b fromHL() { return C_PHL + prefix; }

	inline ssh_b fromRP_AF(ssh_b rp) { return prefAF[(prefix << 3) + (rp & 6)]; }

	inline ssh_b read8() { auto v = memZX[_pc++]; path_ops[posOps++] = v; return v; }

	inline ssh_w read16() { auto v = *(ssh_w*)(memZX + _pc); _pc += 2; *(ssh_w*)(path_ops + posOps) = v; posOps += 2; return v; }

	inline ssh_w addrReg(ssh_b reg) { return 0; }

	// B C D E H L (HL) A
	inline void fromRON1(ssh_b ron) { auto r = prefRON[ron]; adr = addrReg(r); DA_PUT(r); }
	
	// (HL/IX+d/IY+d)/B C D E H/IXH/IYH L/IXL/IYL
	void fromRON(ssh_b ron) {
		if(ron == 6) {
			auto r = fromHL();
			adr = addrReg(r);
			DA_PUT(r);
			if(prefix) { d = read8(); DA_PUT(C_D); adr += (char)d; }
		} else {
			auto r = prefRON[(prefix << 3) + ron];
			adr = addrReg(r);
			DA_PUT(r);
		}
		DA_PUT(C_ADDR);
		DA_PUT(C_PADDR);
	}

	ssh_b prefix;
	ssh_b typeOps;
	ssh_b codeOps;
	ssh_b groupOps;
	int pos, posOps;
	ssh_b path[32];
	ssh_b path_ops[32];
};

typedef void(zxDisAsm::*funcDA)();
