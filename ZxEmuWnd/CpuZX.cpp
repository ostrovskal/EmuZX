
#include "stdafx.h"
#include "CpuZX.h"
#include "DecoderZX.h"
#include "zxDebugger.h"

extern ssh_cws nameROMs[];

// содержмое памяти
ssh_b memZX[65536];

// регистры
ssh_b regsZX[COUNT_REGS];

// порты
ssh_b portsZX[65536];

// банки памяти
ssh_b memBanks[8 * 16384];

ssh_b* limitROM = &memZX[16384];
ssh_b* limitScreen = &memZX[23296];

// адрес прерывания
ssh_b* _I = nullptr;

// ренегерация
ssh_b* _R = nullptr;

// режим прерываний
ssh_b* _IM = nullptr;

// триггеры
ssh_b* _IFF1 = nullptr;
ssh_b* _IFF2 = nullptr;

// содержимое порта FE
ssh_b* _PORT_FE = nullptr;
ssh_b* _PORT_FD = nullptr;

// ROM
ssh_b* ptrROM = nullptr;
int szROM = 0;

// RAM
ssh_b* _RAM = nullptr;

// VID
ssh_b* _VID = nullptr;
ssh_b* memScreen = nullptr;

// ROM
ssh_b* _ROM = nullptr;

ssh_b* _A = nullptr;
ssh_b* _B = nullptr;

ssh_w* _BC = nullptr;
ssh_w* _DE = nullptr;
ssh_w* _HL = nullptr;
ssh_w* _SP = nullptr;
ssh_w* _IX = nullptr;
ssh_w* _IY = nullptr;
ssh_w* _PC = nullptr;
ssh_w* _PC_EXIT_CALL = nullptr;

// скан строка
ssh_b* _SCAN = nullptr;

// статус
volatile ssh_w _TSTATE = 0;

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
	_IM= &regsZX[IM];

	_IFF1 = &regsZX[IFF1];
	_IFF2 = &regsZX[IFF2];
	_PORT_FE = &regsZX[FE];
	_PORT_FD = &regsZX[FD];
	_RAM = &regsZX[RAM];
	_ROM = &regsZX[ROM];
	_VID = &regsZX[VID];
	_SCAN = &regsZX[SCAN];

	_BC = (ssh_w*)&regsZX[RC];
	_DE = (ssh_w*)&regsZX[RE];
	_HL = (ssh_w*)&regsZX[RL];
	_SP = (ssh_w*)&regsZX[RSPL];
	_PC = (ssh_w*)&regsZX[RPCL];
	_IX = (ssh_w*)&regsZX[RXL];
	_IY = (ssh_w*)&regsZX[RYL];
	_PC_EXIT_CALL = (ssh_w*)&regsZX[PC_EXIT_CALL1];

	*_R = *_I = *_IM = 0;
	*_IFF1 = *_IFF2 = 0;
	*_PORT_FE = 224;
	*_PORT_FD = 192;
	*_RAM = *_ROM = *_VID = *_SCAN = 0;
	*_PC = *_BC = *_DE = *_HL = *_IX = *_IY = 0;

	SAFE_DELETE(ptrROM); szROM = 0;

	auto model = theApp.getOpt(OPT_MEM_MODEL)->dval;
	// грузим ПЗУ
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
	if(_IFF1 && *_IFF1 && (_TSTATE & ZX_TRAP)) {
		modifyTSTATE(0, ZX_TRAP);
		*_IFF1 = *_IFF2 = 0;
		switch(*_IM) {
			case 0: 
				// вызываем системный монитор (debug window)
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
			if(theApp.debug->checkBPS(*_PC, false)) return;
		}
		// выполнение операции
		decoder->execOps(0, 0);
		// проверка на прерывания
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

			theApp.getOpt(OPT_MEM_MODEL)->dval = model;
			
			if(_read(_hf, &regsZX, sizeof(regsZX)) != sizeof(regsZX)) throw(0);
			if(_read(_hf, memZX, 65536) != 65536) throw(0);
			if(_read(_hf, memBanks, 16384 * 8) != 16384 * 8) throw(0);

			l -= (3 + sizeof(regsZX) + 16384 * 12);
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
