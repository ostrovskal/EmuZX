#pragma once

class DecoderZX;
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

	// ��������/���������� ���������
	bool loadStateZX(const StringZX& path);
	bool saveStateZX(const StringZX& path);

	bool loadZ80(const StringZX& path);
	bool saveZ80(const StringZX& path);

	// ����������
	DecoderZX* decoder;

	// ��������� ������
	static ssh_b memory[65536];

	// ���
	ssh_b* ROM;
	long szROM;

	static bool isExec;

	// ��������
	static ssh_b IFF1;
	static ssh_b IFF2;

	// ������
	static ssh_b STATE;

	// ���������� ����� FE
	static ssh_b portFE;

	// ������� ������� ���������� INT
	static ssh_b trap;

	// ����������
	static ssh_b RI;

	// �����������
	static ssh_b RR;

	// ����� ����������
	static ssh_b IM;

	// ������� �������
	static ssh_w PC;

	// ��������
	static ssh_b cpu[22];

	// �����
	static ssh_b port[65536];

	static ssh_b* A;
	static ssh_b* B;

	static ssh_w* BC;
	static ssh_w* DE;
	static ssh_w* HL;
	static ssh_w* SP;
};
