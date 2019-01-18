
#include "stdafx.h"
#include "CpuZX.h"
#include "DecoderZX.h"
#include "BorderZX.h"
#include "SoundZX.h"
#include "KeyboardZX.h"

int CpuZX::frames = 0;

int CpuZX::keyd = 0;
int CpuZX::keyu = 0;

byte CpuZX::IFF1 = 0;
byte CpuZX::IFF2 = 0;
byte CpuZX::trap = 0;

bool CpuZX::isExec = false;

byte CpuZX::STATE = 0;

byte CpuZX::border = 7;
byte CpuZX::sound = 0;

byte CpuZX::RI;

byte CpuZX::RR;

byte CpuZX::IM;

word CpuZX::RPC;

byte* CpuZX::A;
byte* CpuZX::B;

word* CpuZX::BC;
word* CpuZX::DE;
word* CpuZX::HL;
word* CpuZX::SP;

byte CpuZX::cpu[22];

byte CpuZX::memory[65536];

byte CpuZX::port[65536];

CpuZX::CpuZX() {
	decoder = new DecoderZX();
	gpu = new GpuZX();
	brd = new BorderZX();
	key = new KeyboardZX();
	snd = new SoundZX();

	decoder->gpu = gpu;
	brd->gpu = gpu;

	signalRESET();
}

CpuZX::~CpuZX() {
	delete decoder;
	delete gpu;
	delete brd;
	delete snd;
	delete key;

	decoder = nullptr;
	gpu = nullptr;
	brd = nullptr;
	key = nullptr;
	snd = nullptr;
}

void CpuZX::signalRESET() {
	RPC = 0;
	RR = RI = IM = trap = 0;
	IFF1 = IFF2 = 0;
	STATE = 0;
	border = 7;
	sound = 0;
	frames = 0;
	isExec = true;
	GpuZX::frames = 0;
	BorderZX::frames = 0;

	memset(cpu, 0, sizeof(cpu));
	memset(memory, 0, sizeof(memory));
	memset(port, 255, sizeof(port));

	A = &cpu[RA];
	B = &cpu[RB];

	BC = (word*)&cpu[RC];
	DE = (word*)&cpu[RE];
	HL = (word*)&cpu[RL];
	SP = (word*)&cpu[RSPL];

	*SP = 65534;

	// грузим ПЗУ
	int hh;
	if(_wsopen_s(&hh, L"c:\\ZX.rom", _O_RDONLY, _SH_DENYRD, _S_IREAD) != -1) {
		_read(hh, memory, 16384);
		_close(hh);
	}

	byte exam[] = {
		33, 0, 64,
		54, 0b11101101,
		62, 0b10101010,
		237, 111,
		62, 0x35,
		6, 0x47,
		144,
		39,
		15, 15, 15,
		33, 0, 64,
		17, 1, 64,
		1, 255, 26, 
		52,
		237, 176, 
		24, 242
	};
	//memcpy(memory, exam, sizeof(exam));
}

void CpuZX::signalINT() {
	if(IFF1 && trap) {
		trap = IFF1 = IFF2 = 0;
		switch(IM) {
			case 0: 
				// вызываем системный монитор (debug window)
				break;
			case 1:
				decoder->execCALL(0x38);
				break;
			case 2:
				decoder->execCALL(decoder->read_mem16(RI * 256 + 254));
				break;
		}
	}
}

void CpuZX::signalNMI() {
	IFF1 = 0;
	decoder->execCALL(0x66);
}

void CpuZX::execute() {
	if(isExec) {
		frames++;
		STATE |= INT;
		// выполнение операции
		decoder->execOps(0, 0);
		// проверка на прерывания
		if((STATE & INT) == INT) signalINT();
	}
}

void CpuZX::updateGpu() {
	// отображение графики
	gpu->execute();
}

void CpuZX::redrawBorder() {
	brd->execute();
}

void CpuZX::playSound() {
	snd->execute();
}

void CpuZX::updateKey(int k, bool pressed) {
	key->execute(k, pressed);
}

bool CpuZX::saveStateZX(const wchar_t* path) {
	int h = 0;

	isExec = false;

	try {
		if((_wsopen_s(&h, path, _O_CREAT | _O_TRUNC | _O_WRONLY | _O_BINARY, _SH_DENYWR, _S_IWRITE)) != -1) {

			if(_write(h, &cpu, sizeof(cpu)) != sizeof(cpu)) throw(0);
			if(_write(h, &memory, sizeof(memory)) != sizeof(memory)) throw(0);

			byte temp[32];

			temp[0] = RI; temp[1] = RR; temp[2] = IM; temp[3] = IFF1; temp[4] = IFF2;
			temp[5] = STATE; temp[6] = border; temp[7] = sound; temp[8] = trap;
			*(word*)(temp + 9) = RPC;

			int size =	sizeof(RI) + sizeof(RR) + sizeof(IM) + sizeof(IFF1) + sizeof(IFF2) +
						sizeof(STATE) + sizeof(border) + sizeof(sound) + sizeof(trap) + sizeof(RPC);
			
			if(_write(h, &temp, size) != size) throw(0);
			if(_write(h, &port, sizeof(port)) != sizeof(port)) throw(0);

			_close(h);
		}
	} catch(...) {
		if(h != 0) _close(h);
		return false;
	}

	isExec = true;

	return true;
}

bool CpuZX::loadStateZX(const wchar_t* path) {
	int hh = 0;
	
	isExec = false;
	try {
		if(_wsopen_s(&hh, path, _O_RDONLY | _O_BINARY, _SH_DENYRD, _S_IREAD) != -1) {

			if(_read(hh, &cpu, sizeof(cpu)) != sizeof(cpu)) throw(0);
			if(_read(hh, &memory, sizeof(memory)) != sizeof(memory)) throw(0);
			
			int size =	sizeof(RI) + sizeof(RR) + sizeof(IM) + sizeof(IFF1) + sizeof(IFF2) + 
						sizeof(STATE) + sizeof(border) + sizeof(sound) + sizeof(trap) + sizeof(RPC);
			
			if(_read(hh, &port, size) != size) throw(0);

			RI = port[0]; RR = port[1]; IM = port[2]; IFF1 = port[3]; IFF2 = port[4];
			STATE = port[5]; border = port[6]; sound = port[7]; trap = port[8];
			RPC = *(word*)(port + 9);
			
			if(_read(hh, &port, sizeof(port)) != sizeof(port)) throw(0);

			_close(hh);

			CpuZX::STATE |= CpuZX::BORDER;
		}
	} catch(...) {
		if(hh != 0) _close(hh);
		return false;
	}
	
	isExec = true;
	return true;
}