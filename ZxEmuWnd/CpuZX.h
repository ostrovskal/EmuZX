#pragma once

class DecoderZX;
class CpuZX {
public:
	CpuZX();
	virtual ~CpuZX();

	void signalRESET();
	void signalINT();
	void signalNMI();

	// выполнение
	void execute();

	// загрузка/сохранение состояния
	bool loadStateZX(const StringZX& path);
	bool saveStateZX(const StringZX& path);

	// дешифратор
	DecoderZX* decoder;

	// ПЗУ
	ssh_b* ROM;
	ssh_d szROM;
};
