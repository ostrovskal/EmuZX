
#include "stdafx.h"
#include "CpuZX.h"
#include "DecoderZX.h"
#include "SettingsZX.h"

extern SettingsZX settings;
extern ssh_cws nameROMs[];

// содержмое памяти
ssh_b* memZX;
ssh_d szMemZX;

// регистры
ssh_b regsZX[22];

// порты
ssh_b portsZX[65536];

// счетчик комманд
ssh_w _PC;

// адрес прерывания
ssh_b _I;

// ренегерация
ssh_b _R;

// режим прерываний
ssh_b _IM;

// триггеры
ssh_b _IFF1;
ssh_b _IFF2;

// статус
ssh_b _STATE;

// содержимое порта FE
ssh_b _PORT_FE;

// признак наличия прерывания INT
ssh_b _TRAP;

ssh_b* _A;
ssh_b* _B;

ssh_w* _BC;
ssh_w* _DE;
ssh_w* _HL;
ssh_w* _SP;

CpuZX::CpuZX() {
	decoder = new DecoderZX();
	ROM = nullptr; szROM = 0;
	memZX = nullptr; szMemZX = 0;
	signalRESET();
}

CpuZX::~CpuZX() {
	_STATE &= ~ZX_EXEC;
	delete decoder; decoder = nullptr;
	delete ROM; ROM = nullptr;
	delete memZX; memZX = nullptr;
}

void CpuZX::signalRESET() {
	_STATE = 0;
	_PC = 0;
	_R = _I = _IM = _TRAP = 0;
	_IFF1 = _IFF2 = 0;
	_PORT_FE = 224;

	delete ROM; ROM = nullptr;
	delete memZX; memZX = nullptr;
	szROM = 0;

	_A = &regsZX[RA];
	_B = &regsZX[RB];

	_BC = (ssh_w*)&regsZX[RC];
	_DE = (ssh_w*)&regsZX[RE];
	_HL = (ssh_w*)&regsZX[RL];
	_SP = (ssh_w*)&regsZX[RSPL];

	// грузим ПЗУ
	int hh = 0;
	try {
		StringZX pathROM(settings.mainDir + L"Roms\\" + nameROMs[settings.modelZX] + L".rom");
		_wsopen_s(&hh, pathROM, _O_RDONLY, _SH_DENYRD, _S_IREAD);
		if(hh) {
			szROM = _filelength(hh);
			ROM = new ssh_b[szROM];
			if(_read(hh, ROM, szROM) != szROM) throw(0);
		}
	} catch(...) {

	}
	if(hh) _close(hh);

	switch(settings.modelZX) {
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
			szMemZX = 65536 * 2;
			memZX = new ssh_b[szMemZX];
			memset(memZX, 0, szMemZX);
			memcpy(memZX, ROM, 16384);
			break;
	}

	memset(regsZX, 0, sizeof(regsZX));
	memset(portsZX, 255, sizeof(portsZX));

	portsZX[31] = 0;

	_STATE |= ZX_EXEC;
}

void CpuZX::signalINT() {
	if(_IFF1 && _TRAP) {
		_TRAP = _IFF1 = _IFF2 = 0;
		decoder->incrementR();
		switch(_IM) {
			case 0: 
				// вызываем системный монитор (debug window)
				decoder->execCALL(0x66);
				break;
			case 1:
				decoder->execCALL(0x38);
				break;
			case 2:
				decoder->execCALL(decoder->read_mem16(_I * 256 + 254));
				break;
		}
	}
}

void CpuZX::signalNMI() {
	_IFF1 = 0;
	decoder->execCALL(0x66);
}

void CpuZX::execute() {
	if((_STATE & ZX_EXEC) == ZX_EXEC) {
		_STATE |= ZX_INT;
		// выполнение операции
		decoder->execOps(0, 0);
		// проверка на прерывания
		if((_STATE & ZX_INT) == ZX_INT) signalINT();
	}
}

bool CpuZX::saveStateZX(const StringZX& path) {
	int h = 0;

	bool result = true;
	auto state = _STATE;
	
	_STATE &= ZX_EXEC;

	try {
		_wsopen_s(&h, path, _O_CREAT | _O_TRUNC | _O_WRONLY | _O_BINARY, _SH_DENYWR, _S_IWRITE);
		if(h) {
			ssh_b temp[32];

			temp[0] = _I; temp[1] = _R; temp[2] = _IM; temp[3] = _IFF1; temp[4] = _IFF2;
			temp[5] = _STATE; temp[6] = _PORT_FE; temp[7] = _TRAP; temp[8] = settings.modelZX;
			*(ssh_w*)(temp + 9) = _PC;
			*(ssh_d*)(temp + 11) = szMemZX;

			int size =	sizeof(_I) + sizeof(_R) + sizeof(_IM) + sizeof(_IFF1) + sizeof(_IFF2) +
						sizeof(_STATE) + sizeof(_PORT_FE) + sizeof(_TRAP) + sizeof(settings.modelZX) +
						sizeof(_PC) + sizeof(szMemZX);
			
			if(_write(h, &temp, size) != size) throw(0);
			if(_write(h, &regsZX, sizeof(regsZX)) != sizeof(regsZX)) throw(0);
			if(_write(h, memZX, szMemZX) != szMemZX) throw(0);
		}
	} catch(...) {
		result = false;
	}

	if(h) _close(h);

	_STATE = state;

	return result;
}

bool CpuZX::loadStateZX(const StringZX& path) {
	FILE* hh = nullptr;
	bool result = true;
	
	auto state = _STATE;
	_STATE &= ~ZX_EXEC;

	try {
		_wfopen_s(&hh, path, L"rb");
		if(hh) {
			int size =	sizeof(_I) + sizeof(_R) + sizeof(_IM) + sizeof(_IFF1) + sizeof(_IFF2) + 
						sizeof(_STATE) + sizeof(_PORT_FE) + sizeof(_TRAP) + sizeof(_PC) +
						sizeof(szMemZX) + sizeof(settings.modelZX);
			
			ssh_b temp[32];

			if(fread(&temp, 1, size, hh) != size) throw(0);

			_I = temp[0]; _R = temp[1]; _IM = temp[2]; _IFF1 = temp[3]; _IFF2 = temp[4];
			_STATE = temp[5]; _PORT_FE = temp[6]; _TRAP = temp[7]; settings.modelZX = temp[8];
			_PC = *(ssh_w*)(temp + 9);
			szMemZX = *(ssh_d*)(temp + 11);
			
			delete memZX; memZX = new ssh_b[szMemZX];

			if(fread(&regsZX, 1, sizeof(regsZX), hh) != sizeof(regsZX)) throw(0);
			if(fread(memZX, 1, szMemZX, hh) != szMemZX) throw(0);
		}
	} catch(...) {
		result = false;
	}
	
	if(hh) fclose(hh);

	_STATE = (state | (ZX_BORDER | ZX_SOUND));

	return result;
}

