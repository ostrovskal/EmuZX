#pragma once

class DecoderZX;
class CpuZX {
public:
	CpuZX();
	virtual ~CpuZX();

	// ����������
	void execute(bool run_debugger);

	// ��������/���������� ���������
	bool loadStateZX(const StringZX& path);
	bool saveStateZX(const StringZX& path);

	// ����������
	DecoderZX* decoder;
protected:
	void signalRESET();
	void signalINT();
	void signalNMI();

	// ���
	ssh_b* ROM;
	ssh_d szROM;
};
