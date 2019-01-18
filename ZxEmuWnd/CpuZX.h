#pragma once

#include "GpuZX.h"

class DecoderZX;
class GpuZX;
class BorderZX;
class KeyboardZX;
class SoundZX;
class CpuZX {
public:
	enum {
		INT		= 1,
		BORDER	= 2,
		SOUND	= 4
	};
	CpuZX();
	virtual ~CpuZX();

	void signalRESET();
	void signalINT();
	void signalNMI();

	// ����������
	void execute();

	// ���������� �������
	void updateGpu();

	// ����������� �������
	void redrawBorder();
	
	// ������������ �����
	void playSound();

	// ���������� ������
	void updateKey(int key, bool pressed);
	
	// ���������� ���������
	bool saveStateZX(const wchar_t* path);

	// �������� ���������
	bool loadStateZX(const wchar_t* path);

	// ����������
	DecoderZX* decoder;

	// ����������� �����
	GpuZX* gpu;

	// ������
	BorderZX* brd;

	// ����������
	KeyboardZX* key;

	// �����
	SoundZX* snd;

	// ��������� ������
	static byte memory[65536];

	static int frames;

	static int keyd;
	static int keyu;
	
	static bool isExec;

	// ��������
	static byte IFF1;
	static byte IFF2;

	// ������
	static byte STATE;

	// ���� �������
	static byte border;

	// ������ �����
	static byte sound;

	// ������� ������� ���������� INT
	static byte trap;

	// ����������
	static byte RI;

	// �����������
	static byte RR;

	// ����� ����������
	static byte IM;

	// ������� �������
	static word RPC;

	// ��������
	static byte cpu[22];

	// �����
	static byte port[65536];

	static byte* A;
	static byte* B;

	static word* BC;
	static word* DE;
	static word* HL;
	static word* SP;
};
