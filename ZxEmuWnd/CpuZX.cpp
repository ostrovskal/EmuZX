
#include "stdafx.h"
#include "CpuZX.h"
#include "DecoderZX.h"
#include "zxDebugger.h"

extern ssh_cws nameROMs[];

// ��������� ������
ssh_b memZX[65536];

// ��������
ssh_b regsZX[COUNT_REGS];

// �����
ssh_b portsZX[65536];

// ����� ������
ssh_b memBanks[8 * 16384];

ssh_b* limitROM = &memZX[16384];
ssh_b* limitScreen = &memZX[23296];

// ������� �������
ssh_w _PC;

ssh_w* _PC_EXIT_CALL;

// ����� ����������
ssh_b* _I;

// �����������
ssh_b* _R;

// ����� ����������
ssh_b* _IM;

// ��������
ssh_b* _IFF1;
ssh_b* _IFF2;

// ���������� ����� FE
ssh_b* _PORT_FE;
ssh_b* _PORT_FD;

// ROM
ssh_b* ptrROM;
int szROM;

// RAM
ssh_b* _RAM;

// VID
ssh_b* _VID;
ssh_b* memScreen;

// ROM
ssh_b* _ROM;

// ������� ������� ���������� INT
ssh_b* _TRAP;

ssh_b* _A;
ssh_b* _B;

ssh_w* _BC;
ssh_w* _DE;
ssh_w* _HL;
ssh_w* _SP;

// ���� ������
ssh_b* _SCAN;

// ������
volatile ssh_w _TSTATE;

CpuZX::CpuZX() {
	decoder = new DecoderZX();
	ptrROM = nullptr; szROM = 0;
	signalRESET();
}

CpuZX::~CpuZX() {
	SAFE_DELETE(decoder);
	SAFE_DELETE(ptrROM);
}

void CpuZX::signalRESET() {
	modifyTSTATE(0, 0xffff);

	theApp.opts.nameLoadProg = L"BASIC";

	_A = &regsZX[RA];
	_B = &regsZX[RB];
	_R = &regsZX[RR];
	_I = &regsZX[RI];
	_IM= &regsZX[RIM];

	_IFF1 = &regsZX[RIFF1];
	_IFF2 = &regsZX[RIFF2];
	_PORT_FE = &regsZX[RFE];
	_PORT_FD = &regsZX[RFD];
	_RAM = &regsZX[RAM];
	_ROM = &regsZX[ROM];
	_VID = &regsZX[VID];
	_SCAN = &regsZX[SCAN];
	_TRAP = &regsZX[RTRAP];

	_BC = (ssh_w*)&regsZX[RC];
	_DE = (ssh_w*)&regsZX[RE];
	_HL = (ssh_w*)&regsZX[RL];
	_SP = (ssh_w*)&regsZX[RSPL];
	_PC_EXIT_CALL = (ssh_w*)&regsZX[RPC_EXIT_CALL1];

	*_R = *_I = *_IM = *_TRAP = 0;
	*_IFF1 = *_IFF2 = 0;
	*_PORT_FE = 224;
	*_PORT_FD = 192;
	*_RAM = *_ROM = *_VID = *_SCAN = 0;
	_PC = 0;

	SAFE_DELETE(ptrROM); szROM = 0;

	auto model = theApp.getOpt(OPT_MEM_MODEL)->dval;
	// ������ ���
	try {
		StringZX pathROM(theApp.opts.mainDir + L"Roms\\" + nameROMs[model] + ".rom");
		_wsopen_s(&_hf, pathROM, _O_RDONLY | _O_BINARY, _SH_DENYRD, _S_IREAD);
		if(_hf != -1) {
			szROM = _filelength(_hf);
			ptrROM = new ssh_b[szROM];
			if(_read(_hf, ptrROM, szROM) != szROM) throw(0);
		}
	} catch(...) { }
	SAFE_CLOSE1(_hf);

	memset(memZX, 0, 65536);
	memset(memBanks, 0, 16384 * 8);
	memset(regsZX, 0, sizeof(regsZX));
	memset(portsZX, 255, sizeof(portsZX));
	portsZX[31] = 0;

	*_SP = 0;

	memScreen = &memZX[16384];
	memcpy(memZX, ptrROM, 16384);

	modifyTSTATE(ZX_EXEC, 0);
}

void CpuZX::signalINT() {
	if(*_IFF1 && *_TRAP) {
		*_TRAP = *_IFF1 = *_IFF2 = 0;
		switch(*_IM) {
			case 0: 
				// �������� ��������� ������� (debug window)
				decoder->execCALL(0x66);
				break;
			case 1:
				decoder->execCALL(0x38);
				break;
			case 2:
				decoder->execCALL(decoder->read_mem16((*_I) * 256 + 254));
				break;
		}
		decoder->incrementR();
	}
}

void CpuZX::signalNMI() {
	_IFF1 = 0;
	decoder->execCALL(0x66);
}

void CpuZX::execute(bool run_debugger) {
	if((_TSTATE & ZX_RESET) == ZX_RESET) signalRESET();
	if((_TSTATE & ZX_EXEC) || run_debugger) {
		modifyTSTATE(ZX_INT, 0);
		if((_TSTATE & ZX_DEBUG) && !run_debugger) {
			if(theApp.debug->checkBPS(_PC, false)) return;
		}
		// ���������� ��������
		decoder->execOps(0, 0);
		// �������� �� ����������
		if((_TSTATE & ZX_NMI) == ZX_NMI) signalNMI();
		else if((_TSTATE & ZX_INT) == ZX_INT) signalINT();
	}
}

bool CpuZX::saveStateZX(const StringZX& path) {
	bool result = false;

	theApp.pauseCPU(true);

	try {
		_wsopen_s(&_hf, path, _O_CREAT | _O_TRUNC | _O_WRONLY | _O_BINARY, _SH_DENYWR, _S_IWRITE);
		if(_hf != -1) {
			result = true;
			ssh_b model = (ssh_b)theApp.getOpt(OPT_MEM_MODEL)->dval;

			if(_write(_hf, &model, 1) != 1) throw(0);
			if(_write(_hf, (ssh_w*)&_TSTATE, 2) != 2) throw(0);
			if(_write(_hf, &_PC, 2) != 2) throw(0);
			if(_write(_hf, &regsZX, sizeof(regsZX)) != sizeof(regsZX)) throw(0);
			if(_write(_hf, memZX, 65536) != 65536) throw(0);
			if(_write(_hf, memBanks, 16384 * 8) != 16384 * 8) throw(0);

			StringZX progs = theApp.opts.nameLoadProg;
			auto l = (int)(progs.length() + 1) * 2;
			if(_write(_hf, progs.buffer(), l) != l) throw(0);
		}
	} catch(...) { result = false; }

	SAFE_CLOSE1(_hf);

	theApp.pauseCPU(false);

	return result;
}

bool CpuZX::loadStateZX(const StringZX& path) {
	bool result = false;

	theApp.pauseCPU(true);

	try {
		_wsopen_s(&_hf, path, _O_RDONLY | _O_BINARY, _SH_DENYRD, _S_IREAD);
		if(_hf != -1) {
			result = true;
			ssh_b model;

			auto l = _filelength(_hf);
			if(_read(_hf, &model, 1) != 1) throw(0);
			if(_read(_hf, (ssh_w*)&_TSTATE, 2) != 2) throw(0);
			if(_read(_hf, &_PC, 2) != 2) throw(0);

			theApp.getOpt(OPT_MEM_MODEL)->dval = model;
			
			if(_read(_hf, &regsZX, sizeof(regsZX)) != sizeof(regsZX)) throw(0);
			if(_read(_hf, memZX, 65536) != 65536) throw(0);
			if(_read(_hf, memBanks, 16384 * 8) != 16384 * 8) throw(0);

			l -= (5 + sizeof(regsZX) + 16384 * 12);
			if(_read(_hf, tmpBuf, l) != l) throw(0);
			theApp.opts.nameLoadProg = tmpBuf;
		}
	} catch(...) {
		result = false;
	}
	
	SAFE_CLOSE1(_hf);

	theApp.pauseCPU(false);

	return result;
}
