#pragma once

#define FASM_FLAG			1
#define FASM_RP				2
#define FASM_R				4
#define FASM_PRP1			8 // (HL/IX+-d/IY+-d)
#define FASM_PRP2			16 // (BC/DE)
#define FASM_PC				32
#define FASM_PNN			64
#define FASM_NN				128
#define FASM_D				256
#define FASM_BIT			512
#define FASM_LABEL			1024
#define FASM_RADDR			2048// $
#define FASM_RP_AF			4096
#define FASM_RP_SP			8192
#define FASM_R1				16384

class zxAssembler {
public:
	struct ZX_ASM_OPERAND {
		ssh_b prefix;
		ssh_b type;
		int len;
		int flags;
		ssh_cws name;
	};

	struct ZX_ASM_CMD {
		ssh_b prefix;
		int code;
		int len;
		void (zxAssembler::*pfn)(ZX_ASM_CMD* cmd);
		ssh_cws name;
	};
	zxAssembler() : nFlags(-1), nn(0), pos(0), text(nullptr) { memset(code, 0, 8); }
	virtual ~zxAssembler() { }
	int parseCommand(ssh_cws text, int flags, int* posErr);
	void parseOps(ZX_ASM_CMD* cmd);
	void parseSingl(ZX_ASM_CMD* cmd);
	void parseIncDec(ZX_ASM_CMD* cmd);
	void parseJump(ZX_ASM_CMD* cmd);
	void parseArifth(ZX_ASM_CMD* cmd);
	void parseCB(ZX_ASM_CMD* cmd);
	void parseIm(ZX_ASM_CMD* cmd);
	void parseLd(ZX_ASM_CMD* cmd);
	void parsePushPop(ZX_ASM_CMD* cmd);
	void parsePort(ZX_ASM_CMD* cmd);
	void parseEx(ZX_ASM_CMD* cmd);
	ssh_b code[8];
protected:
	ZX_ASM_CMD* parseLexem();
	void skipSpace();
	bool skipComma();
	bool isEOF();
	ZX_ASM_OPERAND* parseOperand(int flags);
	ssh_cws text;
	int pos;
	int nFlags;
	int nn;// NN/PNN/N/D/BIT
};
