
#include "stdafx.h"
#include "CpuZX.h"
#include "DecoderZX.h"
#include "zxDebugger.h"

extern ssh_cws nameROMs[];

// содержмое памяти
ssh_b* memZX;
ssh_d szMemZX;

// регистры
ssh_b regsZX[COUNT_REGS];

// порты
ssh_b portsZX[65536];

// счетчик комманд
ssh_w _PC;

// адрес прерывания
ssh_b* _I;

// ренегерация
ssh_b* _R;

// режим прерываний
ssh_b* _IM;

// триггеры
ssh_b* _IFF1;
ssh_b* _IFF2;

// содержимое порта FE
ssh_b* _PORT_FE;

// признак наличия прерывания INT
ssh_b* _TRAP;

ssh_b* _A;
ssh_b* _B;

ssh_w* _BC;
ssh_w* _DE;
ssh_w* _HL;
ssh_w* _SP;

// статус
volatile ssh_w _TSTATE;

CpuZX::CpuZX() {
	decoder = new DecoderZX();
	ROM = nullptr; szROM = 0;
	memZX = nullptr; szMemZX = 0;
	signalRESET();
}

CpuZX::~CpuZX() {
	SAFE_DELETE(decoder);
	SAFE_DELETE(ROM);
	SAFE_DELETE(memZX);
}

void CpuZX::signalRESET() {
	modifyTSTATE(0, 0xffff);

	_A = &regsZX[RA];
	_B = &regsZX[RB];
	_R = &regsZX[RR];
	_I = &regsZX[RI];
	_IM= &regsZX[RIM];

	_IFF1 = &regsZX[RIFF1];
	_IFF2 = &regsZX[RIFF2];
	_PORT_FE = &regsZX[RFE];
	_TRAP = &regsZX[RTRAP];

	_BC = (ssh_w*)&regsZX[RC];
	_DE = (ssh_w*)&regsZX[RE];
	_HL = (ssh_w*)&regsZX[RL];
	_SP = (ssh_w*)&regsZX[RSPL];

	*_R = *_I = *_IM = *_TRAP = 0;
	*_IFF1 = *_IFF2 = 0;
	*_PORT_FE = 224;
	_PC = 0;

	SAFE_DELETE(ROM); szROM = 0;
	SAFE_DELETE(memZX); szMemZX = 0;

	auto model = theApp.getOpt(OPT_MEM_MODEL)->dval;
	// грузим ПЗУ
	try {
		StringZX pathROM(theApp.opts.mainDir + L"Roms\\" + nameROMs[model] + L".rom");
		_wsopen_s(&_hf, pathROM, _O_RDONLY, _SH_DENYRD, _S_IREAD);
		if(_hf) {
			szROM = _filelength(_hf);
			ROM = new ssh_b[szROM];
			if(_read(_hf, ROM, szROM) != szROM) throw(0);
		}
	} catch(...) {

	}
	SAFE_CLOSE1(_hf);

	switch(model) {
		// 48K
		case 0:
			szMemZX = 65536;
			memZX = new ssh_b[szMemZX];
			memset(memZX, 0, szMemZX);
			memcpy(memZX, ROM, 16384);
			*_SP = 65534;
			break;
		// 128K
		case 1:
			szMemZX = 131072;
			memZX = new ssh_b[szMemZX];
			memset(memZX, 0, szMemZX);
			memcpy(memZX, ROM, 16384);
			break;
	}

	memset(regsZX, 0, sizeof(regsZX));
	memset(portsZX, 255, sizeof(portsZX));

	portsZX[31] = 0;

	modifyTSTATE(ZX_EXEC, 0);
}

void CpuZX::signalINT() {
	if(*_IFF1 && *_TRAP) {
		*_TRAP = *_IFF1 = *_IFF2 = 0;
		switch(*_IM) {
			case 0: 
				// вызываем системный монитор (debug window)
				decoder->execCALL(0x66);
				break;
			case 1:
				decoder->execCALL(0x38);
				break;
			case 2:
				decoder->execCALL(decoder->read_mem16(*_I * 256 + 254));
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
		// выполнение операции
		decoder->execOps(0, 0);
		// проверка на прерывания
		if((_TSTATE & ZX_NMI) == ZX_NMI) signalNMI();
		else if((_TSTATE & ZX_INT) == ZX_INT) signalINT();
	}
}

bool CpuZX::saveStateZX(const StringZX& path) {
	bool result = true;

	pauseCPU(true, 0);

	try {
		_wsopen_s(&_hf, path, _O_CREAT | _O_TRUNC | _O_WRONLY | _O_BINARY, _SH_DENYWR, _S_IWRITE);
		if(_hf) {
			ssh_b temp[16];

			temp[0] = (ssh_b)theApp.getOpt(OPT_MEM_MODEL)->dval;
			*(ssh_w*)(temp + 1) = _TSTATE;
			*(ssh_w*)(temp + 3) = _PC;
			*(ssh_d*)(temp + 5) = szMemZX;

			if(_write(_hf, &temp, 9) != 9) throw(0);
			if(_write(_hf, &regsZX, sizeof(regsZX)) != sizeof(regsZX)) throw(0);
			if(_write(_hf, memZX, szMemZX) != szMemZX) throw(0);
		}
	} catch(...) { result = false; }

	SAFE_CLOSE1(_hf);

	pauseCPU(false, 0);

	return result;
}

bool CpuZX::loadStateZX(const StringZX& path) {
	bool result = true;

	pauseCPU(true, 0);

	try {
		_wfopen_s(&hf, path, L"rb");
		if(hf) {
			ssh_b temp[16];

			if(fread(&temp, 1, 9, hf) != 9) throw(0);

			theApp.getOpt(OPT_MEM_MODEL)->dval = temp[0];
			modifyTSTATE(*(ssh_w*)(temp + 1), 0xffff);
			_PC		= *(ssh_w*)(temp + 3);
			szMemZX = *(ssh_d*)(temp + 5);
			
			SAFE_DELETE(memZX);
			memZX = new ssh_b[szMemZX];

			if(fread(&regsZX, 1, sizeof(regsZX), hf) != sizeof(regsZX)) throw(0);
			if(fread(memZX, 1, szMemZX, hf) != szMemZX) throw(0);
		}
	} catch(...) {
		result = false;
	}
	
	SAFE_CLOSE2(hf);

	pauseCPU(false, ZX_BORDER | ZX_SOUND);

	return result;
}
