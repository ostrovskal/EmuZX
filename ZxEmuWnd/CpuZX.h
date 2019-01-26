#pragma once

class DecoderZX;
class CpuZX {
public:
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

	// ����������
	DecoderZX* decoder;

	// ���
	ssh_b* ROM;
	ssh_d szROM;
};
